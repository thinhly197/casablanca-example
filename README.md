Example for C/C++ RESTful API with Casablanca lib

1. Setup environment

a. Install tools
	brew install cmake git openssl libiconv
	brew tap homebrew/dupes
	brew install libiconv
	gem install nokogiri  --with-iconv-dir=/usr/local/Cellar/libiconv/1.14/

b. Compile and Install boost libraries from sources
* Boost
	./bootstrap.sh --prefix=~/Code/lib/boost_1_55 --libdir=~/Code/lib/boost_1_55/lib
	sudo su root
	./b2 install —layout=tagged

c. Compile and Install Casablanca
	git clone https://git.codeplex.com/casablanca
	cd casablanca
	mkdir build.release
	cd build.release
	cmake -DCMAKE_BUILD_TYPE=Debug -G Xcode ../Release
	make -j 4

d. Compile Google Test
	git clone git@github.com:google/googletest.git
  	cd googletest/
  	mkdir build
  	cd build/
  	cmake -Dgtest_build_samples=ON -Dgtest_build_tests=ON /Users/thinhly/googletest/
  	make
  	make test

2. Build
	Open Xcode project by: $ open Server.xcodeproj
	Config header path and libs path for Xcode to the directories which store boost, openssl and cpprest.
	Use Xcode by command: $ xcodebuild -scheme Server -configuration Debug install

	The necessary paths are:
		/usr/local/include
		/usr/local/lib
		/usr/local/opt/openssl/include
		/usr/local/opt/openssl/lib
		<path to folder of>/googletest/googletest/include/
		<path to folder of>/googletest/build/googlemock/gtest

	The necessary libs are:
		-lssl -lcrypto -lboost_system-mt -lcpprest -lboost_chrono-mt -lboost_thread-mt 
		-lgtest

3. Assumptions
	Using Mac OS El Capitan 10.11, and develop with Xcode 7

4. Known Issues

a. Too many warning
	fatal error: too many errors emitted, stopping now [-ferror-limit=]
	-> adding "-Wno-error" and "-Wno-unused-local-typedefs" into CMakeLists.txt

b. Linker error to libraries.
	Compile and install the boost lib by source tar-ball.

5. Unit test:

[==========] Running 5 tests from 1 test case.
[----------] Global test environment set-up.
[----------] 5 tests from UserTest
[ RUN      ] UserTest.getUser
[       OK ] UserTest.getUser (1 ms)
[ RUN      ] UserTest.changePin
[       OK ] UserTest.changePin (3 ms)
[ RUN      ] UserTest.withdrawal
[       OK ] UserTest.withdrawal (4 ms)
[ RUN      ] UserTest.checkBalance
[       OK ] UserTest.checkBalance (3 ms)
[ RUN      ] UserTest.checkTransaction
[       OK ] UserTest.checkTransaction (0 ms)
[----------] 5 tests from UserTest (11 ms total)

[----------] Global test environment tear-down
[==========] 5 tests from 1 test case ran. (11 ms total)
[  PASSED  ] 5 tests.
Program ended with exit code: 0

