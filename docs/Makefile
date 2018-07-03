# Directories 
SRC_DIR ?= ../src
DOC_DIR ?= ./
DOC_BUILD_DIR ?= ./build
DOC_IMG_DIR ?= ./img

DOT_PATH ?= /usr/bin/dot

# General settings
LIBRARY_NAME ?= library
MAINPAGE ?= mainpage.dox
PROJECT_LOGO ?= ./img/ifx_logo.png
LATEX_HEADER ?= ./latexfiles/header.tex
LATEX_FOOTER ?= ./latexfiles/footer.tex
DOXYFILE ?= doxyfile
WORKING_DOXYFILE ?= temp_doxyfile
DOXYGEN_WARNING ?= doxygen.warning

# Tools
CD ?= cd
ECHO ?= echo
DOXYGEN_ECHO ?= echo
RM ?= rm
RM_DIR ?= rm -rf
COPY ?= cp
DOXYGEN ?= /usr/bin/doxygen

lib_doc: clean $(LIBRARY_NAME).pdf
	
$(LIBRARY_NAME).pdf:  
	@$(ECHO) Generating $@ ...
	@$(COPY) $(DOXYFILE) $(WORKING_DOXYFILE)
	@$(DOXYGEN_ECHO) >> $(WORKING_DOXYFILE)
	@$(DOXYGEN_ECHO) INPUT=$(SRC_DIR) >> $(WORKING_DOXYFILE)
	@$(DOXYGEN_ECHO) PROJECT_NAME="Arduino library documentation" >> $(WORKING_DOXYFILE)
	@$(DOXYGEN_ECHO) PROJECT_BRIEF="" >> $(WORKING_DOXYFILE)
	@$(DOXYGEN_ECHO) OUTPUT_DIRECTORY = $(DOC_BUILD_DIR) >> $(WORKING_DOXYFILE)
	@$(DOXYGEN_ECHO) PROJECT_LOGO = $(PROJECT_LOGO) >> $(WORKING_DOXYFILE)
	@$(DOXYGEN_ECHO) IMAGE_PATH = $(DOC_IMG_DIR) >> $(WORKING_DOXYFILE)
	@$(DOXYGEN_ECHO) GENERATE_LATEX = YES >> $(WORKING_DOXYFILE)
	@$(DOXYGEN_ECHO) LATEX_HEADER = $(LATEX_HEADER) >> $(WORKING_DOXYFILE)
	@$(DOXYGEN_ECHO) LATEX_FOOTER = $(LATEX_FOOTER) >> $(WORKING_DOXYFILE)
	@$(DOXYGEN_ECHO) GENERATE_HTML = NO >> $(WORKING_DOXYFILE)
	@$(DOXYGEN_ECHO) LATEX_OUTPUT = latex >> $(WORKING_DOXYFILE)
	@$(DOXYGEN_ECHO) SHOW_USED_FILES = NO >> $(WORKING_DOXYFILE)
	@$(DOXYGEN_ECHO) SHOW_INCLUDE_FILES = NO >> $(WORKING_DOXYFILE)
	@$(DOXYGEN_ECHO) SOURCE_BROWSER = NO >> $(WORKING_DOXYFILE)
	@$(DOXYGEN_ECHO)  >> $(WORKING_DOXYFILE)
	@$(DOXYGEN_ECHO) SHOW_FILES = NO >> $(WORKING_DOXYFILE)
	@$(DOXYGEN_ECHO) DOT_PATH = "$(DOT_PATH)" >> $(WORKING_DOXYFILE)
	@$(RM_DIR) $(DOC_BUILD_DIR)/latex/*
	@$(DOXYGEN) $(WORKING_DOXYFILE)
	
	@$(CD) $(DOC_BUILD_DIR)/latex && pdflatex -interaction=batchmode refman
	@$(CD) $(DOC_BUILD_DIR)/latex && makeindex -q refman.idx
	@$(CD) $(DOC_BUILD_DIR)/latex && pdflatex -interaction=batchmode refman
	@latex_count=8 ; \
	while egrep -s 'Rerun (LaTeX|to get cross-references right)' $(DOC_BUILD_DIR)/latex/refman.log && [ $$latex_count -gt 0 ] ;\
	    do \
	      echo "Rerunning latex...." ;\
	      cd $(DOC_BUILD_DIR)/latex && pdflatex -interaction=batchmode refman ;\
	      latex_count=`expr $$latex_count - 1` ;\
	    done
	@$(CD) $(DOC_BUILD_DIR)/latex && makeindex -q refman.idx
	@$(CD) $(DOC_BUILD_DIR)/latex && pdflatex -interaction=batchmode refman
	@$(COPY) $(DOC_BUILD_DIR)/latex/refman.pdf $@
	@$(RM) $(WORKING_DOXYFILE)

clean:
	@$(ECHO) Removing build directory
	@$(RM_DIR) $(DOC_BUILD_DIR)
	@$(ECHO) Removing temporary files
	@$(RM) $(DOXYGEN_WARNING)
	@$(RM) $(WORKING_DOXYFILE)
	@$(RM) $(LIBRARY_NAME).pdf

.PHONY: lib_doc clean