g++ -c client.cpp -o client.o -Ilws_386/include -g
g++ -c ws_service.cpp -o ws_service.o -Ilws_386/include -g
g++ -c json.cpp -o json.o -Ilws_386/include -g
g++ -c system.cpp -o system.o -g
g++ -c luci.cpp -o luci.o -Ilws_386/include -g
g++ -c util.cpp -o util.o -Ilws_386/include -g
g++ -c controller.cpp -o controller.o -Ilws_386/include -g
g++ -c config.cpp -o config.o -Ilws_386/include -g
g++ -c inparser.cpp -o inparser.o -Ilws_386/include -g
g++ -o test client.o ws_service.o controller.o util.o luci.o inparser.o json.o config.o system.o -Llws_386/lib -lwebsockets jsmn/jsmn.o -lcurl
