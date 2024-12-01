CC      = clang
CFLAGS += -Wall -Wextra -Werror
CFLAGS += -Wno-error=unused-variable
CFLAGS += -Wno-error=unused-but-set-variable
CFLAGS += -I$(shell realpath include)

CXX       = clang++
CXXFLAGS += -Wall -Wextra -Werror -std=c++17
CXXFLAGS += -Wno-error=unused-variable
CXXFLAGS += -Wno-error=unused-but-set-variable
CXXFLAGS += -I$(shell realpath include)

LDFLAGS = -lm -lGL -lglfw

TARGET = opengl-tex
SRC = \
      ./src/main.cpp \
      ./src/stb_image.c
OBJ := $(SRC:.cpp=.o)
OBJ := $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: run
run: $(TARGET)
	./$^

.PHONY: clean
clean:
	find . -type f -iname "*.o" -exec rm "{}" \;

.PHONY: .clangd
.clangd:
	rm --force $@

	@echo Diagnostics: | tee --append $@
	@echo '  UnusedIncludes: Strict' | tee --append $@
	@echo '  MissingIncludes: Strict' | tee --append $@
	@echo CompileFlags: | tee --append $@
	@echo '  Add:' | tee --append $@

	@for flag in $(CFLAGS) $(CXXFLAGS) ; do echo "    - $$flag" | tee --append $@ ; done
