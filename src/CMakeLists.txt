add_subdirectory(res)
add_subdirectory(math)
add_subdirectory(test)
add_subdirectory(core)
add_subdirectory(algo)
add_subdirectory(ca)
add_subdirectory(io)
add_subdirectory(img)
add_subdirectory(rotor)
add_subdirectory(search)

if(BUILD_QT_GUI)
	add_subdirectory(gui_qt)
endif(BUILD_QT_GUI)

if(BUILD_USR_DIR)
	add_subdirectory(usr)
endif(BUILD_USR_DIR)

cp_script(test.sh)
add_test(testscript "bash" "test.sh")

#add_subdirectory(tsa)
