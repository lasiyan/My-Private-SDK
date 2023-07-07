## My-Private-SDK
This is aimed at C++ users working in a cross-compilation environment.
It also wrap standard C++ libraries (like Chrono) to make them easier for users to use.

This will be updated infrequently, but we will keep it up to date.

---

## Test Environment
- **IDE** : Visual studio Code (1.79.2)
- **Target** : CXX Standard version 17
- **CMake** : Required version 3.0

|          | Linux system (ARM 64)  | Windows system (AMD 64) |
| :---:    | :---:                  | :---:                   |
| System   | Jetpack 4.6            | Windows 11 Pro          |
| Detail   | L4T (ubuntu 18.04 LTS) | 22H2. 22621.1848        |
| Compiler | GCC 7.5.0             | MSVC 14.29.30133        |
| (Version)| Ubuntu/Linaro 7.5.0-3ubuntu1~18.04 | Visual Studio 2019 professional |
|

## How to use
1. Compile source or download released version

2. Include `rowen` library to your project
   > Recommend copying the data in ths `install` folder without `bin`

3. Linking
    ```
    # Example. with CMakeLists.txt

    target_include_directories(MyApp PUBLIC path/to/install/rowen/ include)
    target_link_libraries(MyApp path/to/install/lib/librowen.a)
    ```

    ```
    # Example. with pragma comment
    
    // Set the include path in your project settings (if necessary)
    #pragma comment lib ("/path/to/install/librowen.lib")
    ```

4. Using
    ```
    #include <rowen/core.hpp>

    int main()
    {
      auto str1 = rs::format("%s", "hello world !");
      logger.info("%s", str1.c_str());

      auto str2  rs::Time::timeString();
      logger.info("Current Time : %s", str2.c_str());

      return 0;
    }
    ```
  