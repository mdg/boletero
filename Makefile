
ticketeer:
	g++ -o ticketeer ticketeer.cpp

odd: ticketeer-odd

even: ticketeer-even

all: ticketeer odd even

ticketeer-odd:
	g++ -o ticketeer-odd -D MAKE_ODD ticketeer.cpp

ticketeer-even:
	g++ -o ticketeer-even -D MAKE_EVEN ticketeer.cpp
