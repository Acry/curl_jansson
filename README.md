### C - use of libcurl

  !!! ATTENTION SHADERTOY API CURRENTLY NOT WORKING  
  check this link to see if the API is working:  
  https://www.shadertoy.com/api/v1/shaders/MslGWN?key=fdrtwr  

This code retrieves a JSON file from [shadertoy](https://www.shadertoy.com).  

1 - save shader to disk without using API  
2 - save shader to disk with API and conf-file for API-Key  

see: https://www.shadertoy.com/myapps  
I provided a key you can use.  
usage: $name shader-ID  
example:  
`./2 XdjyRm`  
saves:
`XdjyRm.json`  

You can view shaders with https://github.com/Acry/SDL2-OpenGL  
But it is work in progress, just implementing assets, that is why this repo exists.  

3 - parse file with jansson and get some output  
example:  
`./3 XdjyRm.json`  

4 - gets and save needed texture  
example:  
`./4 Ms2SWW`  

8 - Latest working example  

For a working Drag and Drop example see: https://github.com/Acry/SDL2-DnD  

Useful links:  
[libcurl](https://curl.haxx.se/libcurl/) | [jansson](http://www.digip.org/jansson/)  

[More C Example Code](https://gist.github.com/Acry/554e04bab3a2669a5ba2ecd4d673e875)  

[More SDL-C Example Code](https://gist.github.com/Acry/baa861b8e370c6eddbb18519c487d9d8)  

