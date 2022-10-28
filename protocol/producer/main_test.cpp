#include <iostream>
#include <dlfcn.h>

int main() {
    using std::cout;
    using std::cerr;

    cout << "C++ dlopen demo\n\n";

    std::cout << "Name of plugin:" << std::endl;
    std::string plug;
    std::getline(std::cin, plug);
    // open the library
    std::string so = "./";
    so.append(plug);
    so.append(".so");
    cout << "Opening .so...\n";
    void* handle = dlopen(so.c_str(), RTLD_LAZY);
    
    if (!handle) {
        cerr << "Cannot open library: " << dlerror() << '\n';
        return 1;
    }
    
    // load the symbol
    cout << "Loading symbol hello...\n";
    typedef std::string (*hello_t)(std::string);

    // reset errors
    dlerror();
    hello_t hello = (hello_t) dlsym(handle, "function");
    const char *dlsym_error = dlerror();
    if (dlsym_error) {
        cerr << "Cannot load symbol 'function': " << dlsym_error <<
            '\n';
        dlclose(handle);
        return 1;
    }
    
    // use it to do the calculation
    cout << "Calling hello...\n";
    std::string res = hello("s1,s2,s3,/s35");
    std::cout << "res: " << res << std::endl;
    
    // close the library
    cout << "Closing library...\n";
    dlclose(handle);
}
