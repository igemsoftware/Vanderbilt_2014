dardiff_format
==============
* discussion of the development of the heuristic algorithm used to delimit and compress DNA/cDNA files for use by a VCS (version control system)
* called dardiff because it's short for 'darwin diff' and 'windiff' was already taken
# create VCS-formatted file (VCSFMT)

## Problem
If the lines in a VCSFMT'd file are interrelated (if many lines change as a result of a single type of change made to the file), then the diff produced is large, since many lines are added/removed (since diff tools only track changes on the line level, not on the level of characters within a line). Context-appropriate line delimiting is required to ensure the lines of the VCS-formatted file are as non-interrelated as possible.

However, in addition to that level of delimiting, with DNA data even ORFs can be (and typically are) at *least* 900 bp long. With that sort of length, even if only one character on the line is changed, the entire 900bp line has to be diff'd (removed/added with the single character change), which is a ton of bytes to waste. A solution would move character-level changes to the line level so the diff utility can effectively split by line as in traditional VCS.
## Solution

### Line Delimiting
* it should be noted here that the solution is to find the *context-appropriate* solution, not just any random solution
1. line-split by ORFs
	* incredibly simple to code, and simple computationally
	* find start and stop codons appropriate for organism (usually very similar and for MVP can be restricted to the common ones)
	* ORFs can sometimes be very small and completely useless, so most tools to find ORFs provide an option to only find ORFS above some minimum threshold length

# **PROBLEM** : why not just have the VCSFMT'd file have each DNA character delimited by newlines? how do we make this more efficient? can we?
* edge cases that matter are:
	* adding/removing a new ORF (tons of new chars, therefore tons of new lines to diff)
	* adding an entirely new plasmid/genome file
2. other heuristics to provide more granular control

### Nucleotide Compression and by-character differentiation

## Greater Significance
This project could conceivably be used as a starting point for many novel applications of typical version control; as such, great care should be taken to decouple VCSFMT from the integration into CVS. In addition, it may be helpful to consider splitting the two into entirely separate command-line utilities, if that helps with GPL licensing issues.

# integrate file into VCS

## create temp file
The VCSFMT file should be able to completely recreate the original file in its entirety, ideally without any whitespace issues (but lol)


#Problem: deal with insertion of new long lines and single-character changes
###Long line insertions: just split by ORF!
* but then changing individual characters requires inserting/deleting a whole new line: suboptimal

###Single character insertions: just have every character on a new line!
* but then adding long lines makes for a MASSIVE diff with a new line for each character inserted!

###Additional Problem: in a merge, standard diff may put things in place, but out of order!

#Solution
* need (possibly dynamic, but decidable) representation of input that produces the smallest possible (CORRECT) diffs in all of the above situations