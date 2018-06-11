splitArr = data.split("|");
columnCount = parseInt(splitArr[0]);
rowCount = parseInt(splitArr[1]) + 1;
offset = 2;
returnArray = new Array();
for (var i = 0; i < rowCount; i++) {
    rowItems = new Array()
    var index = i * columnCount + offset;
    while (i > 0 && index < splitArr.length && splitArr[index].length == 0)
    {
        returnArray[i - 1] += "|"; 
        offset += 1;
        index += 1;
    }
    for (var j = 0; j < columnCount; j++)
    {
        rowItems.push(splitArr[index])
        index += 1;
    }
    returnArray.push(rowItems.join("|")); 
}
returnArray;