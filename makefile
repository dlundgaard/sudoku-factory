CC 			= g++
CFLAGS		= -Wall

TARGET		= generator

SRC			= src
INCLUDE		= include
BIN			= bin

EXPORT 		= exports

all : clean build run convert print

build :
	@echo -n "🚧 Building... "
	@mkdir -p $(BIN)
	$(CC) $(CFLAGS) -I$(INCLUDE) $(SRC)/$(TARGET).cpp -o $(BIN)/$(TARGET)

run : 
	@echo -n "🚀 Executing... "
	@mkdir -p $(EXPORT)
	./$(BIN)/$(TARGET) $(EXPORT)/export.svg

clean :
	@echo -n "🧹 Cleaning... "
	-rm $(BIN)/*

convert : 
	@echo -n "🖼️  Converting... "
	convert $(EXPORT)/export.svg $(EXPORT)/export.jpg

print : 
	@echo -n "🖨️  Printing... "
	mspaint.exe -pt '$(shell wslpath -w $(EXPORT)/export.jpg)'
