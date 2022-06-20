# test_locks
<pre>
example output for windows pro 64 relase
--
$ ./test_locks.exe
====[]  mutex
task completed in 31826 usec
101   207309
102   272137
103   293753
104   226804
====[]  spin_lock
task completed in 157493 usec
105   317309
106   203101
107   284108
108   195485
====[]  nolock
task completed in 78889 usec
109   854781
110   1264740
111   818168
112   1179624


example output for Linux core 3.13.0-170-generic #220-Ubuntu SMP 
====[]  mutex
task completed in 851608 usec
101   256905
102   252508
103   255502
104   235086
====[]  spin_lock
task completed in 1133972 usec
105   248112
106   237390
107   265554
108   248947
====[]  nolock
task completed in 182396 usec
109   440949
110   383149
111   412888
112   373130

</pre>
