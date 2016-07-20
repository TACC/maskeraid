# Makefile for the Maskeraid document in LaTex format.

version=0.9
Mug =  maskeraid_user_guide
#default: $(Mug).pdf

INTERMEDIATE_FILES=$(Mug).pdf \
                   $(Mug).toc \
                   $(Mug).out \
                   $(Mug).log \
                   $(Mug).aux


#.PHONY : $(Mug).pdf
$(Mug).pdf: 
	rm -f $(INTERMEDIATE_FILES)
	pdflatex -interaction=batchmode -file-line-error $(Mug).tex
	pdflatex -interaction=batchmode -file-line-error $(Mug).tex
	pdflatex -interaction=batchmode -file-line-error $(Mug).tex
	cp $(Mug).pdf $(Mug)-$(version).pdf

clean:
	rm -f $(INTERMEDIATE_FILES)
