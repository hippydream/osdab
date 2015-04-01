# Changelog for Zip/Unzip classes #

<ul>

<li>2012-01-08 Implemented <code>Zip::IgnoreRoot</code> flag; added <code>addFile()</code> and <code>addFiles()</code> convenience methods; added <code>Zip::CheckForDuplicates</code> and <code>Zip::SkipBadFiles</code> flags; added (very) minor improvements.</li>
<li>2011-06-25 Bug entry <a href='http://code.google.com/p/osdab/issues/detail?id=2'>#2</a> - checking of "version needed to extract" flag (thanks to Dominik for reporting).</li>
<li>2011-06-25 Fix compile errors on GCC/Linux.</li>
<li>2011-03-27 Major changes:<br>
<blockquote><ul>
<li>added support for namespace</li>
<li>added support for shared library builds</li>
<li>added basic support for time zones</li>
<li><b>no longer take ownership of <code>QIODevices</code> (MAJOR API CHANGE!)</b></li>
<li>unzip will update last modified time on win32 and posix compliant OS</li>
<li>code clean up</li>
</ul>
</li>
<li>2010-07-08 Bug fix: <code>extractFile()</code> was permanently updating the compressed file size in the zip entry in case of an encrypted entry. Thanks to Serge Kolokolkin for finding and reporting the exact bug.</li>
<li>2008-09-07 Bug fix: rare failure to locate EOCD when archive had a comment.</li>
<li>2007-02-01 New <code>IgnorePaths</code> compression option and two more <code>addDirectoryContents()</code> convenience methods to use this option.</li>
<li>2007-01-28 Major changes:<br>
<ul>
<blockquote><li>bug fix: there was a big problem with directory names</li>
<blockquote><li>API changes: the <code>Zip::addDirectory()</code> method is now easier to use; the password can now be set using a <code>setPassword()</code> method and a new flag allows to preserve absolute paths</li>
<li>added an <code>encrypted</code> flag to the <code>Unzip::ZipEntry</code> struct</li>
<li>removed <code>QObject</code> inheritance. Internationalization is now achieved through <code>QCoreApplication::translate()</code></li>
</blockquote></blockquote></ul>
</li>
<li>2006-11-24 Bug fix: under certain circumstances, an additional directory was being created in the root directory of the zip file.</li>
<li>2006-10-23 Minor API changes; <code>QIODevice</code> support added; better binary compatibility; "long long" issue with older compilers solved.</li>
<li>2006-06-09 Minor API changes.</li>
<li>2005-10-03 First public release.</li>
</ul>