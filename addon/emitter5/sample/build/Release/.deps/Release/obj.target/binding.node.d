cmd_Release/obj.target/binding.node := g++ -shared -pthread -rdynamic -m32  -Wl,-soname=binding.node -o Release/obj.target/binding.node -Wl,--start-group Release/obj.target/binding/sample.o Release/obj.target/binding/plugin/echo.o Release/obj.target/binding/plugin/helper.o Release/obj.target/binding/plugin/plugin.o -Wl,--end-group -Wl,-rpath /home/globik/kore.io_websocket/uv3 -L /home/globik/kore.io_websocket/uv3 -luv_callback -lee -lglib-2.0
