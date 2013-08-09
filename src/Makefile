CC      = gcc
PROF    =
NOCRYPT = 
C_FLAGS = -O -Wall -ggdb $(PROF) $(NOCRYPT)
L_FLAGS = -O -lcrypt $(PROF)

O_FILES = act_comm.o act_info.o act_move.o act_obj.o act_wiz.o class.o comm.o const.o	\
          db.o fight.o handler.o hunt.o interp.o magic.o os.o random.o save.o social.o	\
	  specials.o update.o war.o

merc: $(O_FILES)
	rm -f merc
	$(CC) $(L_FLAGS) -o merc $(O_FILES)

.c.o: merc.h
	$(CC) -c $(C_FLAGS) $<

clean:
	touch *.[ch]
	make

tgz:
	tar czhf ../src_pk.tgz *.[ch] Makefile startup ../area/classes.txt ../area/socials.txt
