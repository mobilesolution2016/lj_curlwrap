# lj_curlwrap
A wrap for libcurl in windows/linux for luajit

ֱ�Ӱ�װ��CURL�⣬��Windows�²�ȡ��LoadLibraryȻ���ȡ�����CURL������ͨ��ָ�뺯�����д���ķ�ʽ��ʡȥ�˶�CURL��Windows�鷳�ı����ʹ�ÿ�Ĺ���
Linuxx����ֱ��ʹ��libcurl.a����

ĿǰΪ�Ƚϳ�����״̬�������ǰ�װ��CURL��һ���Թ��ܣ����Ѿ������ϴ��ļ��ˣ�Ҳ������Json��Text�����͵���������POST����
��δ֧�ֶϵ��������ϵ��ϴ���PUT��DELETE����ȸ��߼������ԡ�

�����Ҫ�첽ִ�У����Կ��ǵķ�ʽ�ǲ���LuaJIT���߳̿���ʵ���첽

	require('strext')
	require('curlwrap')

	--��򵥵���ʽ������һ����ַ����ȡHTML
	local ok, response = CURL.get('http://www.baidu.com')
	--if ok then
		print(response)
	--end

	--����һ��ʵ�����ò�����������
	local c = CURL.new()

	--�����޸�header
	c:addheader('Content-Type', 'utf-8')
	--һ����ݵĺ���ר�����޸�User-Agent
	c:setborwser('chrome')  --c:setborwser('firefox')
	--����ύ������̫�󣬻��߷��ص�̫�󣬿��������Զ�Dump���ļ�
	c:set('dumpupload', 'path/to/filename')
	c:set('dumpresponse', 'path/to/filename')
	c:set('verbose', true)

	--��application/json��ʽ���������б����Json�ַ��������������Զ���ת����
	c:setjson(string.json({ a = 1, b = 'Hello' }, string.JSON_UNICODES))

	--����Post����
	c:setpost({ a = 1, b = 'Hello' })

	--�ϴ��ļ�
	c:addfile('varname', 'd:/1.txt')
	c:addvar('varname', 'any string data here')

	--���ϼ�������ʽֻ��ѡ��һ����������֮������
	local ok, result = c:run('http://xxxxxx')
	if ok then
		print(result.status)
		print(result.response)
	else
		print(result)
	end