o:.Q.def[`objdir`callback`freq`eg!(`$"./lib/";`normal;1000i;`);.Q.opt[.z.x]]
\c 3000 3000

usage:"usage: q q/blog.q -eg eg3 [-freq 100] [-callback slow]" 
if[ null o[`eg];0N!"Must specify example";0N!usage;exit[0]];



o[`obj]: hsym  `$"/" sv string o[`objdir`eg]
startserver:o[`obj] 2:(`startserver;2)
stopserver:o[`obj]  2:(`stopserver;1)
startclient:o[`obj] 2:(`startclient;1)
getstats:o[`obj]    2:(`getstats;1)

normal:{[x;y] 0N!("Update ";.z.t;x;y;getstats[])}
slow:{[x;y] 0N!("Update "; .z.t;x;y;getstats[]);do[100000; sums  til 1000]}
f:value o[`callback]



//Start exchange server 
startserver[1234i;o[`freq]];

system["sleep 1"];

//Create connection.
startclient[1234];

