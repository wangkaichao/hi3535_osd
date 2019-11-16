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
	mkdir -p lib
	make -C hisi_040
	make -C commLib
	make -C paramLib
	make -C osd
	make -C NetLib
	make -C demo

.PHONY:clean
clean:
	make clean -C demo
	make clean -C commLib
	make clean -C paramLib
	make clean -C osd
	make clean -C NetLib
	make clean -C hisi_040
