Doesnt work yet. Should compile using libdragon docker.

TODO:
* Textures dont work
* Performance is horrible
* No font rendering (SDL_Tff has been stripped - we will need something simpler)
* Entirely untested - likely lots of issues.
* Need improvements to makefile to automate texture conversion and (probably) downsampling of textures. We need to handle PNG and JPG textures and convert to the .sprite libdragon format and put in the filesystem folder.
* Were hitting 64MB limit already, need to be smart about what assets we include and downsampling of textures/models etc should help.
* Reduce polys on models 
