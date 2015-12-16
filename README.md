<h1>Huffman-Codes</h1>

Huffman Coding is a lossless, prefix-style compressional algorithm. The algorithm represents characters in variable length codes. Codes are derived from the frequency of each character's appearance in the file - characters with higher frequencies are given smaller codes. More information about the concept behind the Huffman compression algorithm can be found online, including <a href="https://en.wikipedia.org/wiki/Huffman_coding">here</a>.

In the given implementation, when building the Huffman Tree, when two nodes have equal frequency but differing characters, the character with the greater ASCII value (as seen <a href="http://www.asciitable.com/">here</a>) is considered greater overall. If the node is a parent, the character used for comparison is the right child's character.

<h2>Use</h2>
The provided Make file will compile the two files appropriately. 
<ul>
  <li>Make huffencode - compiles the Huffman Encode file, naming it "<b>encode</b>".</li>
  <li>Make huffdecode - compiles the Huffman Decode file, naming it "<b>decode</b>".</li>
  <li>Make (all) - Compiles both files.</li>
  <li>Make clean - Removes Emacs temp files (i.e. tempFile.c~), test outfile (myOut.txt), and the a.out executable file.</li> 
</ul>

The programs expect the following arguments, respectively:

<ol><li><h4>Huffman Encode</h4>
<p>./encode [file_1] [file_2] <i>where</i></p>
          
  <ul><li><b>file_1</b> is the file to be encoded and</li>
        <li><b>file_2</b> is the file where the encoded output is to be written.</li></ul></p>
</li>             
<li><h4>Huffman Decode</h4>
<p>./decode [file_1] [file_2] <i>where</i></p>
          
  <ul><li><b>file_1</b> is the encoded file to be decoded and</li>
        <li><b>file_2</b> is the file where the decoded output is to be written.</li></ul></p>
</li></ol>
