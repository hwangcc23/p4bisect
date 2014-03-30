p4bisect
========

![screenshot](https://raw.github.com/hwangcc23/p4bisect/master/p4bisect_screenshot.png)

[About]:

Provide the equivalent to git-bisect for Perforce(P4).

[Usage]:

1. Get p4api from http://www.perforce.com/downloads/Perforce/20-User?qt-perforce_downloads_step_3=6#qt-perforce_downloads_step_3#49

2. Build p4bisect via 'P4API=/YOUR_P4API_PATH/p4api-2013.3.784164 make'

3. Login your p4 server
![screenshot](https://raw.github.com/hwangcc23/p4bisect/master/p4_login.png)

4. Use p4bisect to debug submits between 3/28 and 3/30.

   For examp:

   hwangcc23 > p4bisect -f //deopt/project/ -g '@>=2014/03/28' -b '@<=2014/03/30'

5. Based on the binary search method, p4bisect will select the middle revision and sync code. 
![screenshot](https://raw.github.com/hwangcc23/p4bisect/master/p4bisect_screenshot.png)

6. Verify the revision. Assume it is a bad revision, enter 'b' to mark it. p4bisect will find the next revision.
![screenshot](https://raw.github.com/hwangcc23/p4bisect/master/p4bisect_markbad.png)

7. Repeat the step 6 until a revision which the bug is introduced is found.

8. Can enter 's' to switching between "p4 labels" view and "p4 changes" view.
