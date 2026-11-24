CC      = gcc
CFLAGS  = -Wall -Wextra -Iinclude -MMD -MP
SRC     = $(wildcard src/*.c)
OBJ     = $(SRC:src/%.c=build/%.o)

campus: $(OBJ)
	$(CC) $(OBJ) -o campus

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir -p build

run: campus
	./campus

web: campus
	./campus --serve

clean:
	rm -rf build campus campus.dot

.PHONY: run web clean
-include $(OBJ:.o=.d)
