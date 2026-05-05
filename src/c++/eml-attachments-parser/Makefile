all: get_attachments base16 
	g++ get_attachments.o base16.o -o get_attachments

get_attachments:
	g++ -c get_attachments.cpp -o get_attachments.o

base16:
	g++ -c base16.cpp -o base16.o

clean_all: clean
	rm -f get_attachments

clean:
	rm -f get_attachments.o base16.o 