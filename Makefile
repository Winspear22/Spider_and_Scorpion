# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: adnen <adnen@student.42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/02/21 19:55:50 by adnen             #+#    #+#              #
#    Updated: 2026/03/02 18:00:24 by adnen            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= spider

CXX			= c++
# Paths for local curl build
CURL_SRC    = curl-8.6.0
CURL_TAR    = curl-8.6.0.tar.gz
CURL_DIR    = $(CURDIR)/curl_build
LIBCURL     = $(CURL_DIR)/lib/libcurl.a

CXXFLAGS	= -Wall -Wextra -Werror -std=c++17 -I$(CURL_DIR)/include
LIBS		= $(LIBCURL) -lpthread -lssl -lcrypto

SRC_DIR		= ./
OBJ_DIR		= obj/

SRCS		= main.cpp Spider.cpp UseCurl.cpp ArgsParser.cpp HtmlParser.cpp UrlParser.cpp ImageDownloader.cpp
OBJS		= $(SRCS:%.cpp=$(OBJ_DIR)%.o)

# Colors for pretty output
GREEN		= \033[0;32m
RED			= \033[0;31m
RESET		= \033[0m

all: $(NAME)

$(NAME): $(LIBCURL) $(OBJ_DIR) $(OBJS)
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME) $(LIBS)
	@echo "$(GREEN)$(NAME) successfully built!$(RESET)"

$(LIBCURL):
	@if [ ! -d "$(CURL_DIR)" ]; then \
		echo "$(YELLOW)Compiling libcurl locally with OpenSSL support (this may take a minute)...$(RESET)"; \
		rm -rf $(CURL_SRC); \
		tar -xf $(CURL_TAR); \
		cd $(CURL_SRC) && ./configure --prefix=$(CURL_DIR) --with-openssl --without-libpsl --without-libidn2 --disable-shared > /dev/null 2>&1 && \
		make -j$(shell nproc) > /dev/null 2>&1 && make install > /dev/null 2>&1 && \
		echo "$(GREEN)libcurl compiled successfully!$(RESET)"; \
	fi

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@
	@echo "Compiled: $<"

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

clean:
	@rm -rf $(OBJ_DIR)
	@echo "$(RED)Object files cleaned.$(RESET)"

fclean: clean
	@rm -f $(NAME)
	@rm -rf $(CURL_DIR) $(CURL_SRC)
	@echo "$(RED)$(NAME) and local libcurl removed.$(RESET)"

re: fclean all

.PHONY: all clean fclean re
