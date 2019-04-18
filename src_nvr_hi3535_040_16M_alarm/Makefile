################# select sensor type for your sample ####################
####imx290_mipi,/imx290_mipi_double########
####imx117_8m,imx117_1080p,imx117_720p##########
####imx377_8m,imx377_1080p,imx377_720p,imx377_12m##########
####imx274_8m##########
####imx226_8m, imx226_9m, imx226_12m##########
####ov4689_4m_double,ov4689_4m_single##########
#SNS ?= imx290_mipi
#SNS ?= ov4689_4m_single
export SNS

all:
	make -C hisi_040
	make -C commLib
	make -C paramLib
	make -C osd
	#make -C email
	#make -C iwlistLib
	#make -C ptzLib
	#make -C streamLib
	#make -C encLib
	#make -C recordLib
	#make -C ftpLib
	#make -C appLib
	#make -C goodlinkLib
	make -C NetLib
	#make -C fabLib
	make -C demo

.PHONY:clean
clean:
	make clean -C demo
	#make clean -C goodlinkLib
	make clean -C commLib
	#make clean -C iwlistLib
	#make clean -C email
	make clean -C paramLib
	make clean -C osd
	#make clean -C streamLib
	#make clean -C appLib
	#make clean -C encLib
	#make clean -C ftpLib
	make clean -C NetLib
	#make clean -C ptzLib
	#make clean -C recordLib
	#make clean -C fabLib
	make clean -C hisi_040
