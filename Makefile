# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: adnen <adnen@student.42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/02/21 19:55:50 by adnen             #+#    #+#              #
#    Updated: 2026/03/03 00:14:32 by adnen            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME_SPIDER	= spider
NAME_SCORPION	= scorpion

CXX		= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++17

# Paths for local curl build (Required for Spider)
CURL_SRC    	= curl-8.6.0
CURL_TAR    	= curl-8.6.0.tar.gz
CURL_DIR    	= $(CURDIR)/curl_build
LIBCURL     	= $(CURL_DIR)/lib/libcurl.a
SPIDER_FLAGS	= -I$(CURL_DIR)/include
SPIDER_LIBS	= $(LIBCURL) -lpthread -lssl -lcrypto

# Directories
SPIDER_DIR	= Spider/
SCORPION_DIR	= Scorpion/
OBJ_DIR		= obj/

# Source files
SPIDER_SRCS	= main.cpp Spider.cpp UseCurl.cpp ArgsParser.cpp HtmlParser.cpp UrlParser.cpp ImageDownloader.cpp Crawler.cpp
SCORPION_SRCS	= main.cpp ArgsParser.cpp FileValidator.cpp MetaDataReader.cpp ExifParser.cpp

# Object files
SPIDER_OBJS	= $(addprefix $(OBJ_DIR)spider/, $(SPIDER_SRCS:.cpp=.o))
SCORPION_OBJS	= $(addprefix $(OBJ_DIR)scorpion/, $(SCORPION_SRCS:.cpp=.o))

# Colors
GREEN		= \033[0;32m
RED		= \033[0;31m
YELLOW		= \033[0;33m
RESET		= \033[0m

# Rules
all: $(NAME_SPIDER) $(NAME_SCORPION)

# Spider binary
$(NAME_SPIDER): $(LIBCURL) $(OBJ_DIR)spider/ $(SPIDER_OBJS)
	@$(CXX) $(CXXFLAGS) $(SPIDER_FLAGS) $(SPIDER_OBJS) -o $(NAME_SPIDER) $(SPIDER_LIBS)
	@echo "$(GREEN)$(NAME_SPIDER) successfully built!$(RESET)"

# Scorpion binary
$(NAME_SCORPION): $(OBJ_DIR)scorpion/ $(SCORPION_OBJS)
	@$(CXX) $(CXXFLAGS) $(SCORPION_OBJS) -o $(NAME_SCORPION)
	@echo "$(GREEN)$(NAME_SCORPION) successfully built!$(RESET)"

# Libcurl local compilation
$(LIBCURL):
	@if [ ! -d "$(CURL_DIR)" ]; then \
		echo "$(YELLOW)Compiling libcurl locally with OpenSSL support (this may take a minute)...$(RESET)"; \
		rm -rf $(CURL_SRC); \
		tar -xf $(CURL_TAR); \
		cd $(CURL_SRC) && ./configure --prefix=$(CURL_DIR) --with-openssl --without-libpsl --without-libidn2 --disable-shared > /dev/null 2>&1 && \
		make -j$(shell nproc) > /dev/null 2>&1 && make install > /dev/null 2>&1 && \
		echo "$(GREEN)libcurl compiled successfully!$(RESET)"; \
	fi

# Compiling Spider objects
$(OBJ_DIR)spider/%.o: $(SPIDER_DIR)%.cpp
	@$(CXX) $(CXXFLAGS) $(SPIDER_FLAGS) -c $< -o $@
	@echo "Compiled Spider: $<"

# Compiling Scorpion objects
$(OBJ_DIR)scorpion/%.o: $(SCORPION_DIR)%.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@
	@echo "Compiled Scorpion: $<"

# Object directories
$(OBJ_DIR)spider/:
	@mkdir -p $(OBJ_DIR)spider/

$(OBJ_DIR)scorpion/:
	@mkdir -p $(OBJ_DIR)scorpion/

clean:
	@rm -rf $(OBJ_DIR)
	@echo "$(RED)Object files cleaned.$(RESET)"

fclean: clean
	@rm -f $(NAME_SPIDER) $(NAME_SCORPION)
	@rm -rf $(CURL_DIR) $(CURL_SRC)
	@echo "$(RED)Binaries and local libcurl removed.$(RESET)"

re: fclean all

.PHONY: all clean fclean re
