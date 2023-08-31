# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: agras <agras@student.42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/04/10 17:11:19 by trobert           #+#    #+#              #
#    Updated: 2023/08/01 20:53:58 by agras            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	=	webserv

SRCS_FILES	=	main.cpp			\
				Webserv.cpp			\
				Server.cpp			\
				ConfigFile.cpp		\
				ServerContext.cpp	\
				Directive.cpp		\
				utils.cpp			\
				HttpWorker.cpp		\
				headerFields.cpp	\
				ResponseBuilder.cpp	\
				HttpTables.cpp		\
				CGIHandler.cpp		\
				Request.cpp			\
				Connection.cpp		\
				Response.cpp

INC_FILES	= 	Webserv.hpp			\
				Server.hpp			\
				ConfigFile.hpp		\
				ServerContext.hpp	\
				Directive.hpp		\
				directives.hpp		\
				utils.hpp			\
				HttpWorker.hpp		\
				header.hpp			\
				headerFields.hpp	\
				httpDatas.hpp		\
				ResponseBuilder.hpp	\
				HttpTables.hpp		\
				CGIHandler.hpp		\
				Request.hpp			\
				Connection.hpp		\
				Response.hpp

SRCS	=	$(addprefix srcs/, $(SRCS_FILES))

INCS	=	$(addprefix includes/, $(INC_FILES))

CC		=	c++

CFLAGS	=	-Wall -Wextra -Werror -std=c++98 -g3

OBJS_FILES	=	$(SRCS_FILES:%.cpp=%.o)

OBJS	=		$(addprefix objs/, $(OBJS_FILES))

DEP		=		$(OBJS:%.o=%.d)

all		:		$(NAME)

$(NAME)	:	$(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

clean	:
	rm -rf $(OBJS) $(DEP)
	rm -rf objs/

fclean	:	clean
	rm -rf $(NAME)

re		:	fclean all

objs/%.o	: srcs/%.cpp $(INCS)
	mkdir -p objs
	$(CC) $(CFLAGS) -MMD -o $@ -c $<

.PHONY: all clean fclean re
