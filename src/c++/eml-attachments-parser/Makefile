all: get_attachments base16 charset_decoder base64_bytes
	g++ get_attachments.o base16.o base64_bytes.o charset_decoder.o -o get_attachments

get_attachments:
	g++ -c get_attachments.cpp -o get_attachments.o

base16:
	g++ -c base16.cpp -o base16.o

charset_decoder:
	g++ -c charset_decoder.cpp -o charset_decoder.o

base64_bytes:
	g++ -c base64_to_bytes.cpp -o base64_bytes.o

clean_all: clean
	rm -f get_attachments

clean:
	rm -f get_attachments.o base16.o base64_bytes.o charset_decoder.o 