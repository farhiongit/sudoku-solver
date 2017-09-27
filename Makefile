CC				= clang -std=c11
WARNINGS	= -Wall -pedantic-errors -Werror
#For debuging, use DEBUG instead of COMPILE
SUDOKU_SIZE     = 3
COMPILE		= -pipe -O3
#DEBUG			= -g -pg
#For profiling, use DEBUG option instead of COMPILE, run executable, then "gprof ./solveSudoku gmon.out"
#PROC_OPT        = -march=i686
LD_OPT		= -s
CFLAGS  = $(DEBUG) $(WARNINGS) $(COMPILE) $(PROC_OPT) -DSUDOKU_SIZE=$(SUDOKU_SIZE)

SOLVE_C = solve_mask.c
SOLVE_H = solve.h
SRCS    = $(SOLVE_C) terminal.c main.c
HEADERS = $(SOLVE_H) terminal.h
OBJS    = $(SRCS:.c=.o)
EXE     = solveSudoku$(SUDOKU_SIZE)

# Static library, linked with -L. -lsolve if needed
LIB     = libsolve.a
NM_OPT  =--extern-only --defined-only -Col

.PHONY: all
all: $(LIB) $(EXE) messages sudoku.pdf

.PHONY: exe
exe: $(EXE)

.PHONY: lib
lib: $(LIB)

.PHONY: test
test: all
	./sudoku.ksh

.PHONY: messages
messages: SUDOKU_SOLVER

.PHONY: SUDOKU_SOLVER
SUDOKU_SOLVER: po/fr.po po/en_US.po

po/SUDOKU_SOLVER.pot: $(SOLVE_C)
	xgettext --no-wrap --omit-header -LC -ppo -d SUDOKU_SOLVER -k_ $(SOLVE_C)
	@mv po/SUDOKU_SOLVER.po po/SUDOKU_SOLVER.pot

po/fr.po: po/SUDOKU_SOLVER.pot
	(cd po ; msginit --no-wrap --no-translator -lfr_FR.utf8 || msgmerge --verbose -U fr.po SUDOKU_SOLVER.pot ; touch fr.po)

po/en_US.po: po/SUDOKU_SOLVER.pot
	(cd po ; msginit --no-wrap --no-translator -len_US.utf8 || msgmerge --verbose -U en_US.po SUDOKU_SOLVER.pot ; touch en_US.po)

$(EXE): $(OBJS) ../knuth_dancing_links/libdlx.a
	$(CC) $(CFLAGS) $(LD_OPT) -o $@ $(OBJS) ../knuth_dancing_links/libdlx.a  #-L"$(LIBPATH)" -ldlx

terminal.o: terminal.c terminal.h $(SOLVE_H) Makefile
$(SOLVE_C:.c=.o): $(SOLVE_C) $(SOLVE_H) ../knuth_dancing_links/dancing_links.h finally.h Makefile
main.o: main.c $(SOLVE_H) terminal.h Makefile
$(OBJS) : Makefile

$(LIB): $(SOLVE_C:.c=.o)
	@rm -f "$(LIB)"
	ar rcs "$(LIB)" "$(SOLVE_C:.c=.o)"
	@nm $(NM_OPT) "$(LIB)"

.PHONY: clean
clean:
	rm -f $(OBJS) $(LIB) $(EXE) core *~

sudoku.pdf: $(SRCS) $(HEADERS)
	doxygen sudoku.doxygen > /dev/null
	(cd sudoku.doc/latex ; make > /dev/null ; mv refman.pdf ../../sudoku.pdf ; cd -)
