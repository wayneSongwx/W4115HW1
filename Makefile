lexer: lexer.h lexer.cpp token.h
	g++ -o lexer main.cpp lexer.cpp

.PHONY: clean

test: lexer
	for file in test/input/*.minilang; do \
		filename=$${file##*/}; \
		echo "Running test $$filename"; \
		mkdir -p test/result; \
		./lexer "$$file" > test/result/$$filename.out 2> test/result/$$filename.err; \
	done
	python3 test/testing.py test/result test/output test/err


clean:
	rm -f lexer
	rm -rf test/result