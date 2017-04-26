#ifndef _OCR_WINCURLWRAP_H__
#define _OCR_WINCURLWRAP_H__

#include <curl/curl.h>

enum ContentType
{
	kContentNotSet,
	kContentURLEncoded,
	kContentMultiEncData,
	kContentText,
	kContentJson
};

extern "C" {
#ifdef _WINDOWS
	typedef CURL *(*fn_curl_easy_init)(void);
	typedef CURLcode(*fn_curl_easy_setopt)(CURL *curl, CURLoption option, ...);
	typedef CURLcode(*fn_curl_easy_perform)(CURL *curl);
	typedef void(*fn_curl_easy_cleanup)(CURL *curl);
	typedef CURLcode(*fn_curl_easy_getinfo)(CURL *curl, CURLINFO info, ...);
	typedef struct curl_slist *(*fn_curl_slist_append)(struct curl_slist *, const char *);
	typedef void(*fn_curl_slist_free_all)(struct curl_slist *);

	typedef CURLFORMcode (*fn_curl_formadd)(struct curl_httppost ** firstitem, struct curl_httppost ** lastitem, ...);
	typedef void (*fn_curl_formfree)(struct curl_httppost * form);

	typedef CURLcode(*fn_curl_global_init)(long);
	typedef void(*fn_curl_global_cleanup)(void);

	typedef const char* (*fn_curl_easy_strerror)(CURLcode err);
#else
	#define p_curl_easy_init curl_easy_init
	#define p_curl_easy_setopt curl_easy_setopt
	#define p_curl_easy_perform curl_easy_perform
	#define p_curl_easy_cleanup curl_easy_cleanup
	#define p_curl_easy_getinfo curl_easy_getinfo
	#define p_curl_slist_append curl_slist_append
	#define p_curl_slist_free_all curl_slist_free_all

	#define p_curl_formadd curl_formadd
	#define p_curl_formfree curl_formfree

	#define p_curl_global_init curl_global_init
	#define p_curl_global_cleanup curl_global_cleanup
	#define p_curl_easy_strerror curl_easy_strerror
#endif
}

#ifdef _WINDOWS
static fn_curl_easy_init p_curl_easy_init;
static fn_curl_easy_setopt p_curl_easy_setopt;
static fn_curl_easy_perform p_curl_easy_perform;
static fn_curl_easy_cleanup p_curl_easy_cleanup;
static fn_curl_easy_getinfo p_curl_easy_getinfo;
static fn_curl_slist_append p_curl_slist_append;
static fn_curl_slist_free_all p_curl_slist_free_all;
static fn_curl_formadd p_curl_formadd;
static fn_curl_formfree p_curl_formfree;
static fn_curl_easy_strerror p_curl_easy_strerror;
#else
	#define SOCKET int
#endif

class CURLUserData {
public:
	SOCKET				curlSocket;
	CURL				*curl;
	double				dNow, dTotal, dProgressNow;
	char				szBoundary[32];
	ContentType			kCntType;
	struct curl_slist	*appendHeaders;
	struct curl_httppost *formpost;
	std::string			strUpload;
	std::string			strResponse;

public:
	CURLUserData()
		: curlSocket(-1)
		, kCntType(kContentNotSet)
		, formpost(NULL), appendHeaders(NULL)
		, curl(NULL), dProgressNow(0), dNow(0), dTotal(0)
	{
		memset(szBoundary, 0, sizeof(szBoundary));
	}

	virtual ~CURLUserData()
	{
		if (appendHeaders)
			p_curl_slist_free_all(appendHeaders);
		if (formpost)
			p_curl_formfree(formpost);
		if (curl)
			p_curl_easy_cleanup(curl);
	}

	virtual void setProgress(double d)
	{
		dProgressNow = d;
	}
} ;


struct MIMETypes
{
	typedef MAP_CLASSNAME<std::string, char*> MIMETypesMap;
	MIMETypesMap all;

	MIMETypes()
	{
		char szFileExts[] = { "x3d;3gp;3g2;mseq;pwn;plb;psb;pvb;tcap;7z;abw;ace;acc;acu;atc;adp;aab;aam;aas;air;swf;fxp;pdf;ppd;dir;xdp;xfdf;aac;ahead;azf;azs;azw;ami;N/A;apk;cii;fti;atx;mpkg;aw;les;swi;s;atomcat;atomsvc;atom, xml;ac;aif;avi;aep;dxf;dwf;par;bcpio;bin;bmp;torrent;cod;mpm;bmi;sh;btif;rep;bz;bz2;csh;c;cdxml;css;cdx;cml;csml;cdbcmsg;cla;c4g;sub;cdmia;cdmic;cdmid;cdmio;cdmiq;c11amc;c11amz;ras;dae;csv;cpt;wmlc;cgm;ice;cmx;xar;cmc;cpio;clkx;clkk;clkp;clkt;clkw;wbs;cryptonote;cif;cmdf;cu;cww;curl;dcurl;mcurl;scurl;car;pcurl;cmp;dssc;xdssc;deb;uva;uvi;uvh;uvm;uvu;uvp;uvs;uvv;dvi;seed;dtb;res;ait;svc;eol;djvu;dtd;mlp;wad;dpg;dra;dfac;dts;dtshd;dwg;geo;es;mag;mmr;rlc;exi;mgz;epub;eml;nml;xpr;xif;xfdl;emma;ez2;ez3;fst;fvt;fbs;fe_launch;f4v;flv;fpx;npx;flx;fli;ftc;fdf;f;mif;fm;fh;fsc;fnc;ltf;ddd;xdw;xbd;oas;oa2;oa3;fg5;bh2;spl;fzs;g3;gmx;gtw;txd;ggb;ggt;gdl;gex;gxt;g2w;g3w;gsf;bdf;gtar;texinfo;gnumeric;kml;kmz;gqf;gif;gv;gac;ghf;gim;grv;gtm;tpl;vcg;h261;h263;h264;hpid;hps;hdf;rip;hbci;jlt;pcl;hpgl;hvs;hvd;hvp;sfd-hdstx;stk;hal;html;irm;sc;ics;icc;ico;igl;ief;ivp;ivu;rif;3dml;spot;igs;i2g;cdy;xpw;fcs;ipfix;cer;pki;crl;pkipath;igm;rcprofile;irp;jad;jar;class;jnlp;ser;java;js;json;joda;jpm;jpeg, jpg;jpgv;ktz;mmd;karbon;chrt;kfo;flw;kon;kpr;ksp;kwd;htke;kia;kne;sse;lasxml;latex;lbd;lbe;jam;123;apr;pre;nsf;org;scm;lwp;lvp;m3u;m4v;hqx;portpkg;mgp;mrc;mrcx;mxf;nbp;ma;mathml;mbox;mc1;mscml;cdkey;mwf;mfm;msh;mads;mets;mods;meta4;potm;docm;dotm;mcd;flo;igx;es3;mdb;asf;exe;cil;cab;ims;application;clp;mdi;eot;xls;xlam;xlsb;xltm;xlsm;chm;crd;lrm;mvb;mny;pptx;sldx;ppsx;potx;xlsx;xltx;docx;dotx;obd;thmx;onetoc;pya;pyv;ppt;ppam;sldm;pptm;ppsm;mpp;pub;scd;xap;stl;cat;vsd;wm;wma;wax;wmx;wmd;wpl;wmz;wmv;wvx;wmf;trm;doc;wri;wps;xbap;xps;mid;mpy;afp;rms;tmo;prc;mbk;dis;plc;mqy;msl;txf;daf;fly;mpc;mpn;mj2;mpga;mxu;mpeg;m21;mp4a;mp4;mp4;m3u8;mus;msty;mxml;ngdat;n-gage;ncx;nc;nlu;dna;nnd;nns;nnw;rpst;rpss;n3;edm;edx;ext;gph;ecelp4800;ecelp7470;ecelp9600;oda;ogx;oga;ogv;dd2;oth;opf;qbo;oxt;osf;weba;webm;odc;otc;odb;odf;odft;odg;otg;odi;oti;odp;otp;ods;ots;odt;odm;ott;ktx;sxc;stc;sxd;std;sxi;sti;sxm;sxw;sxg;stw;otf;osfpvg;dp;pdb;p;paw;pclxl;efif;pcx;psd;prf;pic;chat;p10;p12;p7m;p7s;p7r;p7b;p8;plf;pnm;pbm;pcf;pfr;pgn;pgm;png;ppm;pskcxml;pml;ai;pfa;pbd;	RFC 2015;pgp;box;ptid;pls;str;ei6;dsc;psf;qps;wg;qxd;esf;msf;ssf;qam;qfx;qt;rar;ram;rmp;rsd;rm;bed;mxl;musicxml;rnc;rdz;rdf;rp9;jisp;rtf;rtx;link66;rss, xml;shf;st;svg;sus;sru;setpay;setreg;sema;semd;semf;see;snf;spq;spp;scq;scs;sdp;etx;movie;ifm;itp;iif;ipk;tfi;shar;rgb;slt;aso;imp;twd;csp;saf;mmf;spf;teacher;svd;rq;srx;gram;grxml;ssml;skp;sgml;sdc;sda;sdd;smf;sdw;sgl;sm;sit;sitx;sdkm;xo;au;wqd;sis;smi;xsm;bdm;xdm;sv4cpio;sv4crc;sbml;tsv;tiff;tao;tar;tcl;tex;tfm;tei;txt;dxp;sfs;tsd;tpt;mxs;t;tra;ttf;ttl;umj;uoml;unityweb;ufd;uri;utz;ustar;uu;vcs;vcf;vcd;vsf;wrl;vcx;mts;vtu;vis;viv;ccxml;vxml;src;wbxml;wbmp;wav;davmount;woff;wspolicy;webp;wtb;wgt;hlp;wml;wmls;wmlsc;wpd;stf;wsdl;xbm;xpm;xwd;der;fig;xhtml;xml;xdf;xenc;xer;rl;rs;rld;xslt;xop;xpi;xspf;xul;xyz;yaml;yang;yin;zir;zip;zmm;zaz" };

		static std::string strMimeTypes;
		strMimeTypes.reserve(18024);
		strMimeTypes.append("application/vnd.hzn-3d-crossword;video/3gpp;video/3gpp2;application/vnd.mseq;application/vnd.3m.post-it-notes;application/vnd.3gpp.pic-bw-large;application/vnd.3gpp.pic-bw-small;application/vnd.3gpp.pic-bw-var;application/vnd.3gpp2.tcap;application/x-7z-compressed;application/x-abiword;application/x-ace-compressed;application/vnd.americandynamics.acc;application/vnd.acucobol;application/vnd.acucorp;audio/adpcm;application/x-authorware-bin;application/x-authorware-map;application/x-authorware-seg;application/vnd.adobe.air-application-installer-package+zip;application/x-shockwave-flash;application/vnd.adobe.fxp;application/pdf;application/vnd.cups-ppd;application/x-director;application/vnd.adobe.xdp+xml;application/vnd.adobe.xfdf;audio/x-aac;application/vnd.ahead.space;application/vnd.airzip.filesecure.azf;application/vnd.airzip.filesecure.azs;application/vnd.amazon.ebook;application/vnd.amiga.ami;application/andrew-inset;application/vnd.android.package-archive;application/vnd.anser-web-certificate-issue-initiation;application/vnd.anser-web-funds-transfer-initiation;application/vnd.antix.game-component;application/vnd.apple.installer+xml;application/applixware;application/vnd.hhe.lesson-player;application/vnd.aristanetworks.swi;text/x-asm;application/atomcat+xml;application/atomsvc+xml;application/atom+xml;application/pkix-attr-cert;audio/x-aiff;video/x-msvideo;application/vnd.audiograph;image/vnd.dxf;model/vnd.dwf;text/plain-bas;application/x-bcpio;application/octet-stream;image/bmp;application/x-bittorrent;application/vnd.rim.cod;application/vnd.blueice.multipass;application/vnd.bmi;application/x-sh;image/prs.btif;application/vnd.businessobjects;application/x-bzip;application/x-bzip2;application/x-csh;text/x-c;application/vnd.chemdraw+xml;text/css;chemical/x-cdx;chemical/x-cml;chemical/x-csml;application/vnd.contact.cmsg;application/vnd.claymore;application/vnd.clonk.c4group;image/vnd.dvb.subtitle;application/cdmi-capability;application/cdmi-container;application/cdmi-domain;application/cdmi-object;application/cdmi-queue;application/vnd.cluetrust.cartomobile-config;application/vnd.cluetrust.cartomobile-config-pkg;image/x-cmu-raster;model/vnd.collada+xml;text/csv;application/mac-compactpro;application/vnd.wap.wmlc;image/cgm;x-conference/x-cooltalk;image/x-cmx;application/vnd.xara;application/vnd.cosmocaller;application/x-cpio;application/vnd.crick.clicker;application/vnd.crick.clicker.keyboard;application/vnd.crick.clicker.palette;application/vnd.crick.clicker.template;application/vnd.crick.clicker.wordbank;application/vnd.criticaltools.wbs+xml;application/vnd.rig.cryptonote;chemical/x-cif;chemical/x-cmdf;application/cu-seeme;application/prs.cww;text/vnd.curl;text/vnd.curl.dcurl;text/vnd.curl.mcurl;text/vnd.curl.scurl;application/vnd.curl.car;application/vnd.curl.pcurl;application/vnd.yellowriver-custom-menu;application/dssc+der;application/dssc+xml;application/x-debian-package;audio/vnd.dece.audio;image/vnd.dece.graphic;video/vnd.dece.hd;video/vnd.dece.mobile;video/vnd.uvvu.mp4;video/vnd.dece.pd;video/vnd.dece.sd;video/vnd.dece.video;application/x-dvi;application/vnd.fdsn.seed;application/x-dtbook+xml;application/x-dtbresource+xml;application/vnd.dvb.ait;application/vnd.dvb.service;audio/vnd.digital-winds;image/vnd.djvu;application/xml-dtd;application/vnd.dolby.mlp;application/x-doom;application/vnd.dpgraph;audio/vnd.dra;application/vnd.dreamfactory;audio/vnd.dts;audio/vnd.dts.hd;image/vnd.dwg;application/vnd.dynageo;application/ecmascript;application/vnd.ecowin.chart;image/vnd.fujixerox.edmics-mmr;image/vnd.fujixerox.edmics-rlc;application/exi;application/vnd.proteus.magazine;application/epub+zip;message/rfc822;application/vnd.enliven;application/vnd.is-xpr;image/vnd.xiff;application/vnd.xfdl;application/emma+xml;application/vnd.ezpix-album;application/vnd.ezpix-package;image/vnd.fst;video/vnd.fvt;image/vnd.fastbidsheet;application/vnd.denovo.fcselayout-link;video/x-f4v;video/x-flv;image/vnd.fpx;image/vnd.net-fpx;text/vnd.fmi.flexstor;video/x-fli;application/vnd.fluxtime.clip;application/vnd.fdf;text/x-fortran;application/vnd.mif;application/vnd.framemaker;image/x-freehand;application/vnd.fsc.weblaunch;application/vnd.frogans.fnc;application/vnd.frogans.ltf;application/vnd.fujixerox.ddd;application/vnd.fujixerox.docuworks;application/vnd.fujixerox.docuworks.binder;application/vnd.fujitsu.oasys;application/vnd.fujitsu.oasys2;application/vnd.fujitsu.oasys3;application/vnd.fujitsu.oasysgp;application/vnd.fujitsu.oasysprs;application/x-futuresplash;application/vnd.fuzzysheet;image/g3fax;application/vnd.gmx;model/vnd.gtw;application/vnd.genomatix.tuxedo;application/vnd.geogebra.file;application/vnd.geogebra.tool;model/vnd.gdl;application/vnd.geometry-explorer;application/vnd.geonext;application/vnd.geoplan;application/vnd.geospace;application/x-font-ghostscript;application/x-font-bdf;application/x-gtar;application/x-texinfo;application/x-gnumeric;application/vnd.google-earth.kml+xml;application/vnd.google-earth.kmz;application/vnd.grafeq;image/gif;text/vnd.graphviz;application/vnd.groove-account;application/vnd.groove-help;application/vnd.groove-identity-message;application/vnd.groove-injector;application/vnd.groove-tool-message;application/vnd.groove-tool-template;application/vnd.groove-vcard;video/h261;video/h263;video/h264;application/vnd.hp-hpid;application/vnd.hp-hps;application/x-hdf;audio/vnd.rip;application/vnd.hbci;application/vnd.hp-jlyt;application/vnd.hp-pcl;application/vnd.hp-hpgl;application/vnd.yamaha.hv-script;application/vnd.yamaha.hv-dic;application/vnd.yamaha.hv-voice;application/vnd.hydrostatix.sof-data;application/hyperstudio;application/vnd.hal+xml;text/html;application/vnd.ibm.rights-management;application/vnd.ibm.secure-container;text/calendar;application/vnd.iccprofile;image/x-icon;application/vnd.igloader;image/ief;application/vnd.immervision-ivp;application/vnd.immervision-ivu;application/reginfo+xml;text/vnd.in3d.3dml;text/vnd.in3d.spot;model/iges;application/vnd.intergeo;application/vnd.cinderella;application/vnd.intercon.formnet;application/vnd.isac.fcs;application/ipfix;application/pkix-cert;application/pkixcmp;application/pkix-crl;application/pkix-pkipath;application/vnd.insors.igm;application/vnd.ipunplugged.rcprofile;application/vnd.irepository.package+xml;text/vnd.sun.j2me.app-descriptor;application/java-archive;application/java-vm;application/x-java-jnlp-file;application/java-serialized-object;text/x-java-source,java;application/javascript;application/json;application/vnd.joost.joda-archive;video/jpm;image/jpeg;video/jpeg;application/vnd.kahootz;application/vnd.chipnuts.karaoke-mmd;application/vnd.kde.karbon;application/vnd.kde.kchart;application/vnd.kde.kformula;application/vnd.kde.kivio;application/vnd.kde.kontour;application/vnd.kde.kpresenter;application/vnd.kde.kspread;application/vnd.kde.kword;application/vnd.kenameaapp;application/vnd.kidspiration;application/vnd.kinar;application/vnd.kodak-descriptor;application/vnd.las.las+xml;application/x-latex;application/vnd.llamagraphics.life-balance.desktop;application/vnd.llamagraphics.life-balance.exchange+xml;application/vnd.jam;application/vnd.lotus-1-2-3;application/vnd.lotus-approach;application/vnd.lotus-freelance;application/vnd.lotus-notes;application/vnd.lotus-organizer;application/vnd.lotus-screencam;application/vnd.lotus-wordpro;audio/vnd.lucent.voice;audio/x-mpegurl;video/x-m4v;application/mac-binhex40;application/vnd.macports.portpkg;application/vnd.osgeo.mapguide.package;application/marc;application/marcxml+xml;application/mxf;application/vnd.wolfram.player;application/mathematica;application/mathml+xml;application/mbox;application/vnd.medcalcdata;application/mediaservercontrol+xml;application/vnd.mediastation.cdkey;application/vnd.mfer;application/vnd.mfmp;model/mesh;application/mads+xml;application/mets+xml;application/mods+xml;application/metalink4+xml;application/vnd.ms-powerpoint.template.macroenabled.12;application/vnd.ms-word.document.macroenabled.12;application/vnd.ms-word.template.macroenabled.12;application/vnd.mcd;application/vnd.micrografx.flo;application/vnd.micrografx.igx;application/vnd.eszigno3+xml;application/x-msaccess;video/x-ms-asf;application/x-msdownload;application/vnd.ms-artgalry;application/vnd.ms-cab-compressed;application/vnd.ms-ims;application/x-ms-application;application/x-msclip;image/vnd.ms-modi;application/vnd.ms-fontobject;application/vnd.ms-excel;application/vnd.ms-excel.addin.macroenabled.12;application/vnd.ms-excel.sheet.binary.macroenabled.12;application/vnd.ms-excel.template.macroenabled.12;application/vnd.ms-excel.sheet.macroenabled.12;application/vnd.ms-htmlhelp;application/x-mscardfile;application/vnd.ms-lrm;application/x-msmediaview;", 8734);
		strMimeTypes.append("application/x-msmoney;application/vnd.openxmlformats-officedocument.presentationml.presentation;application/vnd.openxmlformats-officedocument.presentationml.slide;application/vnd.openxmlformats-officedocument.presentationml.slideshow;application/vnd.openxmlformats-officedocument.presentationml.template;application/vnd.openxmlformats-officedocument.spreadsheetml.sheet;application/vnd.openxmlformats-officedocument.spreadsheetml.template;application/vnd.openxmlformats-officedocument.wordprocessingml.document;application/vnd.openxmlformats-officedocument.wordprocessingml.template;application/x-msbinder;application/vnd.ms-officetheme;application/onenote;audio/vnd.ms-playready.media.pya;video/vnd.ms-playready.media.pyv;application/vnd.ms-powerpoint;application/vnd.ms-powerpoint.addin.macroenabled.12;application/vnd.ms-powerpoint.slide.macroenabled.12;application/vnd.ms-powerpoint.presentation.macroenabled.12;application/vnd.ms-powerpoint.slideshow.macroenabled.12;application/vnd.ms-project;application/x-mspublisher;application/x-msschedule;application/x-silverlight-app;application/vnd.ms-pki.stl;application/vnd.ms-pki.seccat;application/vnd.visio;video/x-ms-wm;audio/x-ms-wma;audio/x-ms-wax;video/x-ms-wmx;application/x-ms-wmd;application/vnd.ms-wpl;application/x-ms-wmz;video/x-ms-wmv;video/x-ms-wvx;application/x-msmetafile;application/x-msterminal;application/msword;application/x-mswrite;application/vnd.ms-works;application/x-ms-xbap;application/vnd.ms-xpsdocument;audio/midi;application/vnd.ibm.minipay;application/vnd.ibm.modcap;application/vnd.jcp.javame.midlet-rms;application/vnd.tmobile-livetv;application/x-mobipocket-ebook;application/vnd.mobius.mbk;application/vnd.mobius.dis;application/vnd.mobius.plc;application/vnd.mobius.mqy;application/vnd.mobius.msl;application/vnd.mobius.txf;application/vnd.mobius.daf;text/vnd.fly;application/vnd.mophun.certificate;application/vnd.mophun.application;video/mj2;audio/mpeg;video/vnd.mpegurl;video/mpeg;application/mp21;audio/mp4;video/mp4;application/mp4;application/vnd.apple.mpegurl;application/vnd.musician;application/vnd.muvee.style;application/xv+xml;application/vnd.nokia.n-gage.data;application/vnd.nokia.n-gage.symbian.install;application/x-dtbncx+xml;application/x-netcdf;application/vnd.neurolanguage.nlu;application/vnd.dna;application/vnd.noblenet-directory;application/vnd.noblenet-sealer;application/vnd.noblenet-web;application/vnd.nokia.radio-preset;application/vnd.nokia.radio-presets;text/n3;application/vnd.novadigm.edm;application/vnd.novadigm.edx;application/vnd.novadigm.ext;application/vnd.flographit;audio/vnd.nuera.ecelp4800;audio/vnd.nuera.ecelp7470;audio/vnd.nuera.ecelp9600;application/oda;application/ogg;audio/ogg;video/ogg;application/vnd.oma.dd2+xml;application/vnd.oasis.opendocument.text-web;application/oebps-package+xml;application/vnd.intu.qbo;application/vnd.openofficeorg.extension;application/vnd.yamaha.openscoreformat;audio/webm;video/webm;application/vnd.oasis.opendocument.chart;application/vnd.oasis.opendocument.chart-template;application/vnd.oasis.opendocument.database;application/vnd.oasis.opendocument.formula;application/vnd.oasis.opendocument.formula-template;application/vnd.oasis.opendocument.graphics;application/vnd.oasis.opendocument.graphics-template;application/vnd.oasis.opendocument.image;application/vnd.oasis.opendocument.image-template;application/vnd.oasis.opendocument.presentation;application/vnd.oasis.opendocument.presentation-template;application/vnd.oasis.opendocument.spreadsheet;application/vnd.oasis.opendocument.spreadsheet-template;application/vnd.oasis.opendocument.text;application/vnd.oasis.opendocument.text-master;application/vnd.oasis.opendocument.text-template;image/ktx;application/vnd.sun.xml.calc;application/vnd.sun.xml.calc.template;application/vnd.sun.xml.draw;application/vnd.sun.xml.draw.template;application/vnd.sun.xml.impress;application/vnd.sun.xml.impress.template;application/vnd.sun.xml.math;application/vnd.sun.xml.writer;application/vnd.sun.xml.writer.global;application/vnd.sun.xml.writer.template;application/x-font-otf;application/vnd.yamaha.openscoreformat.osfpvg+xml;application/vnd.osgi.dp;application/vnd.palm;text/x-pascal;application/vnd.pawaafile;application/vnd.hp-pclxl;application/vnd.picsel;image/x-pcx;image/vnd.adobe.photoshop;application/pics-rules;image/x-pict;application/x-chat;application/pkcs10;application/x-pkcs12;application/pkcs7-mime;application/pkcs7-signature;application/x-pkcs7-certreqresp;application/x-pkcs7-certificates;application/pkcs8;application/vnd.pocketlearn;image/x-portable-anymap;image/x-portable-bitmap;application/x-font-pcf;application/font-tdpfr;application/x-chess-pgn;image/x-portable-graymap;image/png;image/x-portable-pixmap;application/pskc+xml;application/vnd.ctc-posml;application/postscript;application/x-font-type1;application/vnd.powerbuilder6;application/pgp-encrypted;application/pgp-signature;application/vnd.previewsystems.box;application/vnd.pvi.ptid1;application/pls+xml;application/vnd.pg.format;application/vnd.pg.osasli;text/prs.lines.tag;application/x-font-linux-psf;application/vnd.publishare-delta-tree;application/vnd.pmi.widget;application/vnd.quark.quarkxpress;application/vnd.epson.esf;application/vnd.epson.msf;application/vnd.epson.ssf;application/vnd.epson.quickanime;application/vnd.intu.qfx;video/quicktime;application/x-rar-compressed;audio/x-pn-realaudio;audio/x-pn-realaudio-plugin;application/rsd+xml;application/vnd.rn-realmedia;application/vnd.realvnc.bed;application/vnd.recordare.musicxml;application/vnd.recordare.musicxml+xml;application/relax-ng-compact-syntax;application/vnd.data-vision.rdz;application/rdf+xml;application/vnd.cloanto.rp9;application/vnd.jisp;application/rtf;text/richtext;application/vnd.route66.link66+xml;application/rss+xml;application/shf+xml;application/vnd.sailingtracker.track;image/svg+xml;application/vnd.sus-calendar;application/sru+xml;application/set-payment-initiation;application/set-registration-initiation;application/vnd.sema;application/vnd.semd;application/vnd.semf;application/vnd.seemail;application/x-font-snf;application/scvp-vp-request;application/scvp-vp-response;application/scvp-cv-request;application/scvp-cv-response;application/sdp;text/x-setext;video/x-sgi-movie;application/vnd.shana.informed.formdata;application/vnd.shana.informed.formtemplate;application/vnd.shana.informed.interchange;application/vnd.shana.informed.package;application/thraud+xml;application/x-shar;image/x-rgb;application/vnd.epson.salt;application/vnd.accpac.simply.aso;application/vnd.accpac.simply.imp;application/vnd.simtech-mindmapper;application/vnd.commonspace;application/vnd.yamaha.smaf-audio;application/vnd.smaf;application/vnd.yamaha.smaf-phrase;application/vnd.smart.teacher;application/vnd.svd;application/sparql-query;application/sparql-results+xml;application/srgs;application/srgs+xml;application/ssml+xml;application/vnd.koan;text/sgml;application/vnd.stardivision.calc;application/vnd.stardivision.draw;application/vnd.stardivision.impress;application/vnd.stardivision.math;application/vnd.stardivision.writer;application/vnd.stardivision.writer-global;application/vnd.stepmania.stepchart;application/x-stuffit;application/x-stuffitx;application/vnd.solent.sdkm+xml;application/vnd.olpc-sugar;audio/basic;application/vnd.wqd;application/vnd.symbian.install;application/smil+xml;application/vnd.syncml+xml;application/vnd.syncml.dm+wbxml;application/vnd.syncml.dm+xml;application/x-sv4cpio;application/x-sv4crc;application/sbml+xml;text/tab-separated-values;image/tiff;application/vnd.tao.intent-module-archive;application/x-tar;application/x-tcl;application/x-tex;application/x-tex-tfm;application/tei+xml;text/plain;application/vnd.spotfire.dxp;application/vnd.spotfire.sfs;application/timestamped-data;application/vnd.trid.tpt;application/vnd.triscape.mxs;text/troff;application/vnd.trueapp;application/x-font-ttf;text/turtle;application/vnd.umajin;application/vnd.uoml+xml;application/vnd.unity;application/vnd.ufdl;text/uri-list;application/vnd.uiq.theme;application/x-ustar;text/x-uuencode;text/x-vcalendar;text/x-vcard;application/x-cdlink;application/vnd.vsf;model/vrml;application/vnd.vcx;model/vnd.mts;model/vnd.vtu;application/vnd.visionary;video/vnd.vivo;application/ccxml+xml,;application/voicexml+xml;application/x-wais-source;application/vnd.wap.wbxml;image/vnd.wap.wbmp;audio/x-wav;application/davmount+xml;application/x-font-woff;application/wspolicy+xml;image/webp;application/vnd.webturbo;application/widget;application/winhlp;text/vnd.wap.wml;text/vnd.wap.wmlscript;application/vnd.wap.wmlscriptc;application/vnd.wordperfect;application/vnd.wt.stf;application/wsdl+xml;image/x-xbitmap;image/x-xpixmap;image/x-xwindowdump;application/x-x509-ca-cert;application/x-xfig;application/xhtml+xml;application/xml;application/xcap-diff+xml;application/xenc+xml;application/patch-ops-error+xml;application/resource-lists+xml;application/rls-services+xml;application/resource-lists-diff+xml;application/xslt+xml;application/xop+xml;application/x-xpinstall;application/xspf+xml;application/vnd.mozilla.xul+xml;chemical/x-xyz;text/yaml;application/yang;application/yin+xml;application/vnd.zul;application/zip;application/vnd.handheld-entertainment+xml;application/vnd.zzazz.deck+xml", 9290);

		char* ms, *fs;
		char* fileext = strtok_s(szFileExts, ";", &fs);
		char* mimetype = strtok_s(const_cast<char*>(strMimeTypes.c_str()), ";", &ms);

		while (mimetype)
		{
			all.insert(MIMETypesMap::value_type(fileext, mimetype));

			mimetype = strtok_s(0, ";", &ms);
			fileext = strtok_s(0, ";", &fs);
		}
	}

	char* findMimeType(const char* fileName)
	{
		const char* fext = strrchr(fileName, '.');
		if (fext)
		{
			char lext[64] = { 0 };
			size_t extleng = strlen(fext);
			if (extleng <= 63)
			{
				memcpy(lext, fext + 1, extleng - 1);
				strlwr(lext);

				std::string key(lext);
				MIMETypesMap::iterator ite = all.find(key);
				if (ite != all.end())
					return ite->second;
			}
		}

		return "application/octet-stream";
	}
} ;
static MIMETypes _g_MIMETypes;


static size_t curl_write_tostring(void *ptr, size_t size, size_t nmemb, void *userp)
{
	std::string* pstr = (std::string*)userp;
	pstr->append((char*)ptr, size * nmemb);

	return size * nmemb;
}

static size_t curl_write_tofile(void *ptr, size_t size, size_t nmemb, void *filep)
{
	FILE* f = (FILE*)filep;
	fwrite((char*)ptr, 1, size * nmemb, f);

	return size * nmemb;
}

static curl_socket_t curl_storesocket(CURLUserData *data, curlsocktype purpose, struct curl_sockaddr *addr)
{
	SOCKET sock = socket(addr->family, addr->socktype, addr->protocol);
	data->curlSocket = sock;
	return sock;
}

static int curl_progress(void *userp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	CURLUserData* r = (CURLUserData*)userp;
	if (dlnow && dltotal)
		r->setProgress(std::min(1.0, dlnow / dltotal));
	r->dNow = dlnow;
	r->dTotal = dltotal;

	return 0;
}

static struct curl_slist* curl_setuseragent(struct curl_slist* headers, const char* agent)
{
	if (strcmp(agent, "chrome") == 0)
		return p_curl_slist_append(headers, "User-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/57.0.2987.98 Safari/537.36");
	else if (strcmp(agent, "firefox"))
		return p_curl_slist_append(headers, "User-Agent: Mozilla/s.0 (Windows NT 10.0; Win64; x64; rv:52.0) Gecko/20100101 Firefox/52.0");
	return headers;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static CURL* curlInitOne(CURLUserData* user)
{
	CURL* curl = p_curl_easy_init();
	if (!curl)
		return 0;
	
	p_curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	p_curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);

	if (user)
	{
		user->curl = curl;
		p_curl_easy_setopt(curl, CURLOPT_OPENSOCKETFUNCTION, &curl_storesocket);
		p_curl_easy_setopt(curl, CURLOPT_OPENSOCKETDATA, user);
	}

	return curl;
}

static bool curlSetUrl(CURL* curl, const char* url)
{
	const char* protoEnd = strchr(url, ':');
	if (!protoEnd)
		return false;

	if (protoEnd - url > 5)
		return false;

	int useSSL;
	char proto[8] = { 0 };
	memcpy(proto, url, protoEnd - url);
	strlwr(proto);

	if (strcmp(proto, "http") == 0)
		useSSL = 0;
	else if (strcmp(proto, "https") == 0)
		useSSL = 1;
	else
		return false;

	p_curl_easy_setopt(curl, CURLOPT_URL, url);
	p_curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, useSSL);
	p_curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, useSSL);

	return true;
}

static void curlSetPostData(CURL* curl, const char* posts, size_t postsSize)
{
	p_curl_easy_setopt(curl, CURLOPT_POST, 1);
	if (posts && posts[0])
	{		
		p_curl_easy_setopt(curl, CURLOPT_POSTFIELDS, posts);
		p_curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, postsSize);
	}
}

static void curlSetUploadProgress(CURLUserData* user)
{
	p_curl_easy_setopt(user->curl, CURLOPT_PROGRESSFUNCTION, &curl_progress);
	p_curl_easy_setopt(user->curl, CURLOPT_PROGRESSDATA, user);
	p_curl_easy_setopt(user->curl, CURLOPT_NOPROGRESS, 0);
}

static uint32_t curlAddFileContent(CURLUserData* user, const char* filename, const void* sourceData, size_t datasize, const char* varname)
{
	long fsize = 0;
	FILE* fp = NULL;
	if (!sourceData)
	{
		fp = fopen(filename, "rb");
		if (!fp)
			return 0;

		fseek(fp, 0L, SEEK_END);
		fsize = ftell(fp);
		fseek(fp, 0L, SEEK_SET);
	}
	else
	{
		fsize = datasize;
	}

	if (!user->szBoundary[0])
	{		
		const char szSeed[] = { "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ" };

		srand(time(NULL));
		strcpy(user->szBoundary, "LUAC--");
		for (int i = 6; i < 32; ++ i)
			user->szBoundary[i] = szSeed[rand() % 62];

		char tmpHeader[256] = { 0 };
		tmpHeader[sprintf(tmpHeader, "Content-Type: multipart/form-data; boundary=%s", user->szBoundary)] = 0;
		user->appendHeaders = p_curl_slist_append(user->appendHeaders, tmpHeader);

		p_curl_easy_setopt(user->curl, CURLOPT_POST, 1);

		user->strUpload.reserve(512 * 1024);
	}

	char szTmpBuf[256];
	std::string& cnt = user->strUpload;
	
	if (cnt.length())
		cnt.append("\r\n--", 4);
	else
		cnt.append("--", 2);
	cnt.append(user->szBoundary, sizeof(user->szBoundary));
	cnt.append("\r\n", 2);

	if (filename)
	{	
		const char* mimeType = NULL;
		const char* pureFilename = strrchr(filename, '\\');

		if (!pureFilename)
			pureFilename = strrchr(filename, '/');
		if (pureFilename)
			pureFilename ++;
		else
			pureFilename = filename;

		const char* fileExt = strchr(pureFilename, '.');
		if (fileExt)
			mimeType = _g_MIMETypes.findMimeType(fileExt + 1);
		if (!mimeType)
			mimeType = "application/oct-stream";

		size_t len = sprintf(szTmpBuf, "Content-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\nContent-Type: %s\r\n\r\n", varname, pureFilename, mimeType);
		cnt.append(szTmpBuf, len);
	
		if (fp)
		{
			len = cnt.length();
			cnt.resize(len + fsize);
			fread(const_cast<char*>(cnt.data()) + len, 1, fsize, fp);
			fclose(fp);
		}
		else if (fsize)
		{
			cnt.append((const char*)sourceData, fsize);
		}
	}
	else if (fsize)
	{
		size_t len = sprintf(szTmpBuf, "Content-Disposition: form-data; name=\"%s\"\r\n\r\n", varname);
		cnt.append(szTmpBuf, len);
		cnt.append((const char*)sourceData, fsize);
	}

	return cnt.length();
}

static void curlAppendFormData(CURLUserData* cUser, ...)
{
	va_list args;
	va_start(args, cUser->curl);

	int cc = 0;
	struct curl_httppost *formpost = NULL;
	struct curl_httppost *lastptr = NULL;

	for(;;)
	{
		const char* name = va_arg(args, const char*);
		if (!name)
			break;

		const char* data = va_arg(args, const char*);
		p_curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, name, CURLFORM_COPYCONTENTS, data, CURLFORM_END);

		cc ++;
	}

	p_curl_easy_setopt(cUser->curl, CURLOPT_HTTPPOST, formpost);
	cUser->formpost = formpost;
}

static CURLcode curlExecOne(CURL* curl, std::string& strout, bool autoClean = true)
{
	p_curl_easy_setopt(curl, CURLOPT_WRITEDATA, &strout);
	p_curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &curl_write_tostring);
	CURLcode res = p_curl_easy_perform(curl);
	if (autoClean) p_curl_easy_cleanup(curl);

	return res;
}

static CURLcode curlExecOne(CURL* curl, FILE* fp, bool autoClean = true)
{
	p_curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	p_curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &curl_write_tofile);
	CURLcode res = p_curl_easy_perform(curl);
	if (autoClean) p_curl_easy_cleanup(curl);

	return res;
}

static CURLcode curlExecOne(CURLUserData* user)
{
	bool ug = false;
	if (user->appendHeaders)
	{		
		struct curl_slist* next = user->appendHeaders;
		while(next)
		{
			if (strnicmp(next->data, "User-Agent", 10) == 0)
				ug = true;
			next = next->next;
		}
	}

	if (!ug)
		user->appendHeaders = curl_setuseragent(user->appendHeaders, "chrome");

	p_curl_easy_setopt(user->curl, CURLOPT_HTTPHEADER, user->appendHeaders);

	if (user->szBoundary[0])
	{
		std::string& cnt = user->strUpload;
		cnt.append("\r\n--", 4);
		cnt.append(user->szBoundary, sizeof(user->szBoundary));
		cnt.append("--", 2);

		p_curl_easy_setopt(user->curl, CURLOPT_POST, 1);
		p_curl_easy_setopt(user->curl, CURLOPT_POSTFIELDS, cnt.c_str());
		p_curl_easy_setopt(user->curl, CURLOPT_POSTFIELDSIZE, cnt.length());
	}

	user->strResponse.reserve(16384);
	user->strResponse.clear();
		
	p_curl_easy_setopt(user->curl, CURLOPT_WRITEDATA, &user->strResponse);
	p_curl_easy_setopt(user->curl, CURLOPT_WRITEFUNCTION, &curl_write_tostring);

	CURLcode res = p_curl_easy_perform(user->curl);

	return res;
}

static void curlStop(CURLUserData* user)
{
#ifdef _WINDOWS
	shutdown(user->curlSocket, SD_BOTH);
	closesocket(user->curlSocket);
#else
	shutdown(user->curlSocket, SHUT_RDWR);
	close(user->curlSocket);
#endif
	user->curlSocket = -1;
}

#ifdef _WINDOWS
static HMODULE curlLibInit()
{
	HMODULE hCurl = GetModuleHandleW(L"libcurl.dll");
	if (!hCurl)
	{
		hCurl = LoadLibrary(L"libcurl.dll");
		if (!hCurl)
			return 0;

		//fn_curl_global_init p_curl_global_init = (fn_curl_global_init)GetProcAddress(hCurl, "curl_global_init");
		//if (p_curl_global_init)
		//	p_curl_global_init(CURL_GLOBAL_ALL);
	}

	p_curl_easy_init = (fn_curl_easy_init)GetProcAddress(hCurl, "curl_easy_init");
	p_curl_easy_setopt = (fn_curl_easy_setopt)GetProcAddress(hCurl, "curl_easy_setopt");
	p_curl_easy_perform = (fn_curl_easy_perform)GetProcAddress(hCurl, "curl_easy_perform");
	p_curl_easy_cleanup = (fn_curl_easy_cleanup)GetProcAddress(hCurl, "curl_easy_cleanup");
	p_curl_easy_getinfo = (fn_curl_easy_getinfo)GetProcAddress(hCurl, "curl_easy_getinfo");
	p_curl_slist_append = (fn_curl_slist_append)GetProcAddress(hCurl, "curl_slist_append");
	p_curl_slist_free_all = (fn_curl_slist_free_all)GetProcAddress(hCurl, "curl_slist_free_all");
	p_curl_formadd = (fn_curl_formadd)GetProcAddress(hCurl, "curl_formadd");
	p_curl_formfree = (fn_curl_formfree)GetProcAddress(hCurl, "curl_formfree");
	p_curl_easy_strerror = (fn_curl_easy_strerror)GetProcAddress(hCurl, "curl_easy_strerror");

	return hCurl;
}

static void curlLibUnload(HMODULE h)
{
	//fn_curl_global_cleanup p_curl_global_cleanup = (fn_curl_global_cleanup)GetProcAddress(h, "curl_global_cleanup");
	//if (p_curl_global_cleanup)
	//	p_curl_global_cleanup();
	FreeLibrary(h);
}
#endif

#endif