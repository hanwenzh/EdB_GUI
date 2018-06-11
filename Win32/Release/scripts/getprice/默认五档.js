splitArr = data.split("|");
returnStr = "";
returnStr += "<p>arg0&nbsp;arg1</p>\
<p style='margin:0px;'/>\
<table border='0' width='110' style='margin-left:10px;'>\
<tr style='margin:0px;'>\
<td width='25'>卖五</td>\
<td width='40' style='color:green;'>arg20</td>\
<td width='40' style='color:blue'>arg25</td>\
</tr>\
<tr style='margin:0px;'>\
<td width='25'>卖四</td>\
<td width='40' style='color:green;'>arg19</td>\
<td width='40' style='color:blue'>arg24</td>\
</tr>\
<tr style='margin:0px;'>\
<td width='25'>卖三</td>\
<td width='40' style='color:green;'>arg18</td>\
<td width='40' style='color:blue'>arg23</td>\
</tr>\
<tr style='margin:0px;'>\
<td width='25'>卖二</td>\
<td width='40' style='color:green;'>arg17</td>\
<td width='40' style='color:blue'>arg22</td>\
</tr>\
<tr style='margin:0px;'>\
<td width='25'>卖一</td>\
<td width='40' style='color:green;'>arg16</td>\
<td width='40' style='color:blue'>arg21</td>\
</tr>\
</table>\
<p />\
<table border='0' width='110' style='margin-left:10px;'>\
<tr style='margin:0px;'>\
<td width='25'>买一</td>\
<td width='40' style='color:green;'>arg6</td>\
<td width='40' style='color:red'>arg11</td>\
</tr>\
<tr style='margin:0px;'>\
<td width='25'>买二</td>\
<td width='40' style='color:green;'>arg7</td>\
<td width='40' style='color:red'>arg12</td>\
</tr>\
<tr style='margin:0px;'>\
<td width='25'>买三</td>\
<td width='40' style='color:green;'>arg8</td>\
<td width='40' style='color:red'>arg13</td>\
</tr>\
<tr style='margin:0px;'>\
<td width='25'>买四</td>\
<td width='40' style='color:green;'>arg9</td>\
<td width='40' style='color:red'>arg14</td>\
</tr>\
<tr style='margin:0px;'>\
<td width='25'>买五</td>\
<td width='40' style='color:green;'>arg10</td>\
<td width='40' style='color:red'>arg15</td>\
</tr>\
</table>\
<p />\
<table style='margin-left:10px;'>\
<tr style='margin:0px;'>\
<td width='25'>现价</td>\
<td width='40' style='color:green;'>arg5</td>\
</tr>\
<tr style='margin:0px;'>\
<td width='25'>昨收</td>\
<td width='40' style='color:green;'>arg2</td>\
</tr>\
</table>\
";

returnStr = returnStr.replace("arg10", splitArr[20]);
returnStr = returnStr.replace("arg11", splitArr[21]);
returnStr = returnStr.replace("arg12", splitArr[22]);
returnStr = returnStr.replace("arg13", splitArr[23]);
returnStr = returnStr.replace("arg14", splitArr[24]);
returnStr = returnStr.replace("arg15", splitArr[25]);
returnStr = returnStr.replace("arg16", splitArr[16]);
returnStr = returnStr.replace("arg17", splitArr[17]);
returnStr = returnStr.replace("arg18", splitArr[18]);
returnStr = returnStr.replace("arg19", splitArr[19]);
returnStr = returnStr.replace("arg20", splitArr[20]);
returnStr = returnStr.replace("arg21", splitArr[21]);
returnStr = returnStr.replace("arg22", splitArr[22]);
returnStr = returnStr.replace("arg23", splitArr[23]);
returnStr = returnStr.replace("arg24", splitArr[24]);
returnStr = returnStr.replace("arg25", splitArr[25]);
returnStr = returnStr.replace("arg6", splitArr[6]);
returnStr = returnStr.replace("arg7", splitArr[7]);
returnStr = returnStr.replace("arg8", splitArr[8]);
returnStr = returnStr.replace("arg9", splitArr[9]);
returnStr = returnStr.replace("arg2", splitArr[2]);
returnStr = returnStr.replace("arg5", splitArr[5]);
returnStr = returnStr.replace("arg0", splitArr[0]);
returnStr = returnStr.replace("arg1", splitArr[1]);
returnStr;