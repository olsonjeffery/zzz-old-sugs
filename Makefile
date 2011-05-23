#jsapigen: jsmapping.j.cpp
jsapigen:
	echo 'bleh'

%.j.cpp: %.j
	jsapigen < $(patsubst %.cpp,%,$@) > $@
