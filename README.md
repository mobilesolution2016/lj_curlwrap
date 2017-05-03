# lj_curlwrap
A wrap for libcurl in windows/linux for luajit

直接包装的CURL库，在Windows下采取了LoadLibrary然后获取必须的CURL函数，通过指针函数进行处理的方式，省去了对CURL在Windows麻烦的编译和使用库的过程
Linuxx下则直接使用libcurl.a编译

目前为比较初级的状态，仅仅是包装了CURL的一般性功能，但已经可以上传文件了，也可以用Json、Text等类型的数据来做POST请求。
尚未支持断点续传、断点上传或PUT、DELETE请求等更高级的特性。

如果需要异步执行，可以考虑的方式是采用LuaJIT的线程库来实现异步

	require('strext')
	require('curlwrap')

	--最简单的形式，访问一个网址并获取HTML
	local ok, response = CURL.get('http://www.baidu.com')
	--if ok then
		print(response)
	--end

	--创建一个实例设置参数后再请求
	local c = CURL.new()

	--可以修改header
	c:addheader('Content-Type', 'utf-8')
	--一个便捷的函数专用于修改User-Agent
	c:setborwser('chrome')  --c:setborwser('firefox')
	--如果提交的内容太大，或者返回的太大，可以设置自动Dump到文件
	c:set('dumpupload', 'path/to/filename')
	c:set('dumpresponse', 'path/to/filename')
	c:set('verbose', true)

	--以application/json方式请求（请自行编码好Json字符串，函数不会自动帮转换）
	c:setjson(string.json({ a = 1, b = 'Hello' }, string.JSON_UNICODES))

	--设置Post参数
	c:setpost({ a = 1, b = 'Hello' })

	--上传文件
	c:addfile('varname', 'd:/1.txt')
	c:addvar('varname', 'any string data here')

	--以上几种请求方式只能选其一，设置完了之后运行
	local ok, result = c:run('http://xxxxxx')
	if ok then
		print(result.status)
		print(result.response)
	else
		print(result)
	end