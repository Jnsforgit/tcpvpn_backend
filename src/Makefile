# 最终目标目录
TARGET_PATH = ../bin
TARGET_NAME := anyserver
TARGET := $(TARGET_PATH)/$(TARGET_NAME)
# 需要连接的动态库
LIB_FILE = dl pthread
# 每个动态库名前加-l
LINK_LIB = -l:libzlog.a
LINK_LIB += $(addprefix -l,$(LIB_FILE))
DLL_PATH := -L./thirdparty/zlog
# 编译后的obj目录
OBJ_PATH = ../bin/obj
# 搜索.cpp文件 $(wildcard pattern) 是一个函数，用于匹配指定模式的文件，并返回符合条件的文件列表
rwildcard=$(foreach d,$(wildcard $(addsuffix *,$(1))),$(call rwildcard,$(d)/,$(2)) $(filter $(subst *,%,$(2)),$(d)))
SRCS := $(call rwildcard,..,*.cpp)
# 引用头文件目录
DIR_PATH := $(sort  $(dir $(basename $(SRCS))))
INCLUDE_PATH := -I./thirdparty
INCLUDE_PATH += $(addprefix -I,$(DIR_PATH))
# 编译选项
COMPILE_FLAGS = -std=c++14 -O2 -Wall -g
# 构造连接时需要的obj文件名
LINKOBJS := $(addprefix $(OBJ_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRCS)))))

APP_NAME := anyserver

# 链接目标，最终输出
$(TARGET): $(SRCS:.cpp=.o)
	@echo -e "\033[32mLinking $(TARGET) \033[0m"
	g++ -o $(TARGET_PATH)/$(APP_NAME) $(LINKOBJS) $(LINK_LIB) $(DLL_PATH)
#将所有的.c源代码编译成.o目标文件，%:模式规则，$^代表%.cpp，$@代表%.o
%.o: %.cpp
	@mkdir -p ../bin/obj
	@echo -e "\033[32mcompling $@ \033[0m"
	g++ -o $(OBJ_PATH)/$(addsuffix .o, $(notdir $(basename $@))) -c $^ $(COMPILE_FLAGS) $(INCLUDE_PATH)
clean:
	rm $(LINKOBJS)
	rm $(TARGET_PATH)/$(APP_NAME)
test:
	@echo "$(wildcard $(addsuffix *,..))"
	@echo "INCLUDE_PATH is $(INCLUDE_PATH)"
	@echo "SRCS is $(SRCS)"
	@echo "LINKOBJS is $(LINKOBJS)"
run:
	@cd ../bin && ./$(APP_NAME) -p 8080
debug:
	@cd ../bin && gdb --args ./$(APP_NAME) -p 8080