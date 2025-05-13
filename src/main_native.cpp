#ifdef NATIVE
#include <iostream>

// Simple main function for the native environment
int main(int argc, char *argv[]) {
  std::cout << "Distiller: Native build environment test" << std::endl;
  std::cout << "This build is used primarily for testing" << std::endl;

  // A "successful" run
  return 0;
}
#endif // NATIVE