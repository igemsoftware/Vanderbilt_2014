darwin
======

track changes as a genome evolves over time

####current thought process:

1. stick this onto git somehow. we're not going to reinvent an incredibly well-crafted wheel.
2. git is great at line-based differentation. git is not great at dealing with a single very long string i.e. DNA data.
3. we need some way to differentiate between different logical parts of DNA so we can diff at the finest possible granularity.
4. wait...................AUTOMATIC ANNOTATION!!! extend this idea to split DNA semantically as best as possible


#### questions:
* do we assume the user will track only a single file at a time? because plasmid files are typically monolithic and don't rely upon one another? does that matter?
* do we attempt to diff by character as well, if a user wishes to make a line-item mutation?
* which filetypes do we support?
* will automatic annotation become incredibly cumbersome to use?

### MVP
1. (user) easy, standardized, useful collaborative tracking of changes
2. (user) entire history and changelog easily transferable from place to place
3. (vendor) diffs compressed immensely compared to naive methods
4. (vendor) security increased through hash-checking and decentralized control
    * decentralized: if a single server with data is compromised, other mirrors can convene and determine which is truly correct. like bitcoin's model.
