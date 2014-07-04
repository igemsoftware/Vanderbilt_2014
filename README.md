darwin
======

track changes as a genome evolves over time

merge automatic annotation and version control technologies for the greater good

(this repository will eventually be deleted and reborn as a fork of the git git repository)

# MVP
1. (user) easy, standardized, useful collaborative tracking of changes
2. (user) entire history and changelog easily transferable from place to place
3. (vendor) diffs compressed immensely compared to naive methods
4. (vendor) security increased through hash-checking and decentralized control
    * decentralized: if a single server with data is compromised, other mirrors can convene and determine which is truly correct. like bitcoin's model.

##### current next steps
1. need automatic annotation engine (currently focusing just on prokaryotes, or if required even just E.coli/plasmids. we need to get this out /fast/)
2. need to develop method of integration with git
    1. can add a hook onto the diff method, and all other appropriate methods, which converts file into delimited format according to annotation
        * would need to ensure that files are kept the same on disk so that ApE/whatever can access them easily and biologists aren't confused
            * this would probably require being able to convert to/from the annotated form upon git diffing
        * especially given the above there'll be major performance issues depending upon the automatic annotation library used
        * we CANNOT use something that goes out to the web to check up on data; if necessary we'll keep all the annotation data locally and update it when the application is updated
    2. should keep in mind the ability to work with git gui, too; no need to remake yet another wheel and biologists don't like command lines

## licensing (THIS IS REALLY IMPORTANT IF WE WANT THIS TO EVER GET OFF THE GROUND)
* [reference](http://www.gnu.org/licenses/gpl.html)
* git is licensed under the GPL v2 (and other compatible licenses but mostly that)
* therefore this software must also be licensed under the gpl v2
* however, "executing the software on a computer" does not comprise distribution; otherwise no business would be able to use any gpl tools
* so if some corporation wishes to use this software commercially, they're fine, as long as they don't modify it on their own system and offer that version to consumers
* they'll just need to add modifications back into the original codebase, which is actually totally great
* the automatic annotation engine might also be licensed, but there are few more restrictive than the gpl, so it's highly likely they'll be fine

##### thought process:
1. stick this onto git somehow. we're not going to reinvent an incredibly well-crafted wheel.
2. git is great at line-based differentation. git is not great at dealing with a single very long string i.e. DNA data.
3. we need some way to differentiate between different logical parts of DNA so we can diff at the finest possible granularity.
4. wait...................AUTOMATIC ANNOTATION!!! extend this idea to split DNA semantically as best as possible

##### questions:
* do we assume the user will track only a single file at a time? because plasmid files are typically monolithic and don't rely upon one another? does that matter?
* do we attempt to diff by character as well, if a user wishes to make a line-item mutation?
* which filetypes do we support?
* will automatic annotation become incredibly cumbersome to use?

