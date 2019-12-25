import pathlib
import numpy as np
import os
os.environ["CUDA_VISIBLE_DEVICES"]="-1"
import six.moves.urllib as urllib
import sys
#sys.path.extend(["C:/src/models/research/slim"])
import tarfile
import tensorflow as tf
import zipfile

from collections import defaultdict
from io import StringIO
from matplotlib import pyplot as plt
from PIL import Image
from IPython.display import display

from object_detection.utils import ops as utils_ops
from object_detection.utils import label_map_util
from object_detection.utils import visualization_utils as vis_util

#gpus = tf.config.experimental.list_physical_devices('GPU')
#if gpus:
#  # Restrict TensorFlow to only allocate 1GB of memory on the first GPU
#  try:
#    tf.config.experimental.set_virtual_device_configuration(
#        gpus[0],
#        [tf.config.experimental.VirtualDeviceConfiguration(memory_limit=1024*5)])
#    logical_gpus = tf.config.experimental.list_logical_devices('GPU')
#    print(len(gpus), "Physical GPUs,", len(logical_gpus), "Logical GPUs")
#  except RuntimeError as e:
#    # Virtual devices must be set before GPUs have been initialized
#    print(e)

from webAsmPlay import Texture
from webAsmPlay import ImageFeature
from webAsmPlay import ImageFeatures

# patch tf1 into `utils.ops`
utils_ops.tf = tf.compat.v1

# Patch the location of gfile
tf.gfile = tf.io.gfile

print('here')

def load_model(model_name):
  base_url = 'http://download.tensorflow.org/models/object_detection/'
  model_file = model_name + '.tar.gz'
  model_dir = tf.keras.utils.get_file(
    fname=model_name, 
    origin=base_url + model_file,
    untar=True)

  model_dir = pathlib.Path(model_dir)/"saved_model"

  model = tf.saved_model.load(str(model_dir))
  model = model.signatures['serving_default']

  return model

# List of the strings that is used to add correct label for each box.
PATH_TO_LABELS = 'c:/src/models/research/object_detection/data/mscoco_label_map.pbtxt'
global category_index
category_index = label_map_util.create_category_index_from_labelmap(PATH_TO_LABELS, use_display_name=True)

# If you want to test the code with your images, just add path to the images to the TEST_IMAGE_PATHS.
PATH_TO_TEST_IMAGES_DIR = pathlib.Path('c:/src/models/research/object_detection/test_images')
TEST_IMAGE_PATHS = sorted(list(PATH_TO_TEST_IMAGES_DIR.glob("*.jpg")))
TEST_IMAGE_PATHS

model_name = 'ssd_mobilenet_v1_coco_2017_11_17'
detection_model = load_model(model_name)

print(detection_model.inputs)

print(detection_model.output_dtypes)
print(detection_model.output_shapes)

def run_inference_for_single_image(model, image):
  image = np.asarray(image)
  # The input needs to be a tensor, convert it using `tf.convert_to_tensor`.
  input_tensor = tf.convert_to_tensor(image)
  # The model expects a batch of images, so add an axis with `tf.newaxis`.
  input_tensor = input_tensor[tf.newaxis,...]

  # Run inference
  output_dict = model(input_tensor)

  # All outputs are batches tensors.
  # Convert to numpy arrays, and take index [0] to remove the batch dimension.
  # We're only interested in the first num_detections.
  num_detections = int(output_dict.pop('num_detections'))
  output_dict = {key:value[0, :num_detections].numpy() 
                 for key,value in output_dict.items()}
  output_dict['num_detections'] = num_detections

  # detection_classes should be ints.
  output_dict['detection_classes'] = output_dict['detection_classes'].astype(np.int64)
   
  # Handle models with masks:
  if 'detection_masks' in output_dict:
    # Reframe the the bbox mask to the image size.
    detection_masks_reframed = utils_ops.reframe_box_masks_to_image_masks(
              output_dict['detection_masks'], output_dict['detection_boxes'],
               image.shape[0], image.shape[1])      
    detection_masks_reframed = tf.cast(detection_masks_reframed > 0.5,
                                       tf.uint8)
    output_dict['detection_masks_reframed'] = detection_masks_reframed.numpy()
    
  return output_dict
  
model_name = "mask_rcnn_inception_resnet_v2_atrous_coco_2018_01_28"
masking_model = load_model("mask_rcnn_inception_resnet_v2_atrous_coco_2018_01_28") # Segmentation

def detectObjects(textureID, imageID):
	t = Texture.textureToNdArray(textureID)
	image_np = (t * 255).astype(np.uint8)
	#image_np = Image.fromarray(t)

	global output_dict;
	output_dict = run_inference_for_single_image(detection_model, image_np)
	#output_dict = run_inference_for_single_image(masking_model, image_np)
	
	# Visualization of the results of a detection.
	vis_util.visualize_boxes_and_labels_on_image_array(	image_np,
														output_dict['detection_boxes'],
														output_dict['detection_classes'],
														output_dict['detection_scores'],
														category_index,
														instance_masks=output_dict.get('detection_masks_reframed', None),
														use_normalized_coordinates=True,
														line_thickness=1)

	features = []

	for i in output_dict['detection_boxes']:
		print('box ', i)
		features += [ImageFeature(i)]

	print('num ', len(output_dict['detection_boxes']), len(features))

	f = ImageFeatures()

	for i in range(len(output_dict['detection_scores'])):
		#if output_dict['detection_scores'][i] <= 0.5: break
		print('score ', output_dict['detection_scores'][i], 'class', output_dict['detection_classes'][i], 'box', output_dict['detection_boxes'][i])
		f.add(output_dict['detection_scores'][i], category_index[output_dict['detection_classes'][i]]['name'], output_dict['detection_boxes'][i])

	print('imageID ', imageID)

	ImageFeatures.addFeatures(imageID, f)

	#display(Image.fromarray(image_np))
	#Image.fromarray(image_np).show()
