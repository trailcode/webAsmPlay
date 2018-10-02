#include <iostream>
#include <emscripten/emscripten.h>

extern "C"
{
    void one(char* data, int size) {
        for(int i=0; i<30; i++) {
            std::cout << "Worker " << size << std::endl;
            size_t c = 0;
            for(; c < 100000000; ++c) {}
            char buf[1024];
            sprintf(buf, "ddd%i", c);
            emscripten_worker_respond_provisionally(buf, strlen(buf));
            
        }

        emscripten_worker_respond(0, 0);
    }
}