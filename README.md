darwin
======

track changes as a genome evolves over time (hence the name)

##### intent:
* merge automatic annotation and version control technologies
	* this was benchling's pitch but they're not open source
* create parser to make interim file which is more amenable to standard git diff operation
* if putting in some interim layer above git turns out to work really well, this could be extensible to essentially any data, not just DNA

# MVP
1. (user) easy, standardized, useful collaborative tracking of changes
2. (user) entire history and changelog easily transferable from place to place
3. (vendor) diffs compressed immensely compared to naive methods
4. (vendor) security increased through hash-checking, local storage (100% in-lab), and decentralized control
    * decentralized: if a single server with data is compromised, other mirrors can convene and determine which is truly correct; like bitcoin's model.
* it's not like git, because it's specially made for genomic data (git is not granular enough)
* it's not like existing tools, because they don't have the kind of itemized change tracking that git does (existing tools are too granular) (or just nonfree)
* need to compare to control to establish the above two points

## current next steps
1. need automatic annotation engine (currently focusing just on prokaryotes, or if required even just E.coli/plasmids. we need to get this out /fast/)
	* all automated annotation facilities require access and comparison to databases using big data techniques; this won't work for us since we need speed
	* so we're going to start with just splitting by ORFs (between start/stop codons, between stop/stop codons) and see where that gets us
	* we can attempt multiple heuristics and see which ones get us ['lines' are how we'll split up the output so diff can read it]:
		1. the lines least interdependent on one another (to minimize output of diff)
		2. the best way to handle individual character replacements
	* method of detecting DNA region and not any metadata (should be just "find where AGCT starts" but may require more thought)
	* consider that since DNA data is only AGCT, there can be an *immense* amount of compression possible for diffing
2. need to develop method of integration with git
    1. can add a hook onto the diff method, and all other appropriate methods, which converts file into delimited format according to annotation
        * would need to ensure that files are kept the same on disk so that ApE/whatever can access them easily and biologists aren't confused
            * this requires being able to convert to/from the annotated form upon git diffing
    2. should keep in mind the ability to work with git gui, too; no need to remake yet another wheel and biologists don't like command lines
    3. each individual line is gonna be insanely long (gfp is 238 amino acids, for example, in a single ORF) compared to standard diff, even with annotation and split by ORF
		* potential methods then include marking each individual character change
		* this could potentially get very slow very quickly when attempting to apply diffs
	4. file formats:
		* (from http://benchling.com/demo): Genbank, FASTA, ApE, SnapGene, DNAStrider, SeqBuilder, pDRAW	

## thought process:
1. stick this onto git somehow. we're not going to reinvent an incredibly well-crafted wheel.
2. git is great at line-based differentation. git is not great at dealing with a single very long string i.e. DNA data.
3. we need some way to differentiate between different logical parts of DNA so we can diff at the finest possible granularity.
4. wait...................AUTOMATIC ANNOTATION!!! extend this idea to split DNA semantically as best as possible
5. use heuristics to split single codon string into separate regions, which we'll separate by a newline and spit into git's diff

## questions/notes:
* do we need to fundamentally modify git itself, or can we just wrap git with a few scripts and therefore just use the copy of git already installed?
    * this would mean we wouldn't need to release under the GPL; we'll have to see where that goes
* do we assume the user will track only a single file at a time? because plasmid files are typically monolithic and don't rely upon one another? does that matter?
* do we attempt to diff by character as well, if a user wishes to make a line-item mutation?
* which filetypes do we support?
* will automatic annotation become incredibly cumbersome to use?
* we need to allow switching out annotation software at will, in a modular fashion
    * let's only support a single one right now, but let's write the code so that it doesn't rely on the characteristics of that single program
* we'll need to be able to scale to whole-genome (5Mbp) sequences

## licensing (this is really important if we want this to ever get off the ground)
* [reference](http://www.gnu.org/licenses/gpl.html)
* git is licensed under the GPL v2 (and other compatible licenses but mostly that)
* however, if this software does not modify the git code, but merely creates some layer to call git on (unmodified, without dynamically sharing data), then we're in the clear
* consider merits of gpl, though:
	* so if some corporation wishes to use this software commercially, they're fine, as long as they don't modify it on their own system and offer that version to consumers
	* they'll just need to add modifications back into the original codebase, which is actually totally great
	* the automatic annotation engine might also be licensed
