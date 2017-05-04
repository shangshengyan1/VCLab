<<<<<<< HEAD
#include "..\include\MinEnergy.h"

namespace VCLab
{
	using namespace std;
	/*---------------------------------------------------------------------*/
	/*                       ����Gibbs������                               */
	/*---------------------------------------------------------------------*/
	/*
	void MinEnergy::CalcGE(Phase & Phases, double T)
	double MinEnergy::CalcGF(Phase & Phases, double T)
	double MinEnergy::CalcGFdx(Phase & Phases, double T, int idy)
	double MinEnergy::CalcGFdx2(Phase & Phases, double T, int idy1, int idy2)
	double MinEnergy::CalcPara(int Tnseg, Parameter para, double T)
	double MinEnergy::CalcGtao(double tao, double p, int order)
	*/

	// Calc the Gibbs energy, the first , second derivative of site fraction and temperature
	// mode: 0, 1, 2; 10, 11, 12;
	void MinEnergy::CalcGE(int mode, Phase & Phases, double T)
	{
		//
		int i, j;
		double R = 8.31451;
		double xpro_all, xpro, xdiff;
		double dxpro[10], dxpro2[10];
		double d2xpro[10][10], d2xpro2[10][10], d2xpro3[10][10], d2xpro4[10][10];

		double TC = 0;
		double BMAGN = 0;
		double tao;
		//
		double yfracs[MDim];
		double paravalue[3];

		for (i = 0; i < Phases.yn; i++)
		{
			if(Phases.y[i]<eps)
				yfracs[i] = eps;
			else
				yfracs[i] = Phases.y[i];
		}
		//
		Phases.GF = 0; Phases.SF = 0;
		Phases.dGFt = 0; Phases.d2GFt = 0;
		for (i = 0;i < Phases.vyn;i++)
		{
			Phases.dGFx[i] = 0;
			Phases.d2GFxt[i] = 0;
			for (j = 0;j < Phases.vyn;j++)
			{
				Phases.d2GFx[i][j] = 0;
			}
		}

		// ѭ�������
		for (auto para : Phases.Parameters)
		{
			// �ж��¶�����
			int Tnseg = 0;
			for (i = 0; i < para.T.size() - 1;i++)
			{
				if ((para.T[i] - 0.02)< T && T <(para.T[i + 1] + 0.02))   // T - 0.02����ֹ298.14��298.15�������
					break;
				Tnseg++;
			}
			//
			for (i = 0;i < Phases.vyn;i++)
			{
				dxpro[i] = 0;
				d2xpro[i][i] = 0;
				d2xpro2[i][i] = 0;
				d2xpro3[i][i] = 0;
				d2xpro4[i][i] = 0;

				for (j = i + 1;j < Phases.vyn;j++)
				{
					d2xpro[i][j] = 0; d2xpro[j][i] = 0;
					d2xpro2[i][j] = 0; d2xpro2[j][i] = 0;
					d2xpro3[i][j] = 0; d2xpro3[j][i] = 0;
					d2xpro4[i][j] = 0; d2xpro4[j][i] = 0;
				}
			}
			// ���������ǰ���ϵ��
			xpro_all = 1;
			for (i = 0; i < para.yn; i++)
			{
				xpro_all = xpro_all*yfracs[para.yidc[i]];
			}
			// ���ݲ������ͣ���������
			switch (para.kind)
			{
			case 1:
				//g
				xpro = xpro_all;
				// first derivative
				for (i = 0;i < para.vyn;i++)
				{
					dxpro[para.vyidv[i]] = xpro_all / yfracs[para.vyidc[i]];
					// second derivate
					// for i==j, in case 1, this part is zero
					for (j = i + 1;j < para.vyn;j++)
					{
						d2xpro[para.vyidv[i]][para.vyidv[j]] = dxpro[para.vyidv[i]] / yfracs[para.vyidc[j]];
						d2xpro[para.vyidv[j]][para.vyidv[i]] = d2xpro[para.vyidv[i]][para.vyidv[j]];
					}
				}
				break;
			case 2:  // ��Ԫ�໥���ò���
					 //�ǵ����ڲ���Ԫ�໥���ã���(x1-x2)
				xpro = xpro_all;
				xdiff = yfracs[para.idsub2[0]] - yfracs[para.idsub2[1]];
				xpro = xpro*pow(xdiff, para.order);
				//
				//first derivative
				//�ǵ����ڲ���Ԫ�໥���ã���(x1-x2)
				for (i = 0;i < para.vyn;i++)
				{
					dxpro[para.vyidv[i]] = xpro_all/yfracs[para.vyidc[i]]*pow(xdiff, para.order);
				}
				//�ڶ�����, idy���໥���õ��ǵ����ڲ���Xi*Xj*Xk(Xi-Xj)^n��idy��Xi����Xj
				dxpro2[para.vidsub2[0]] = 0;
				if (para.order > 0)  // n=0,��֮��Ϊ0�������ǡ�
				{
					dxpro2[para.vidsub2[0]] = xpro_all*para.order*pow(xdiff, para.order - 1);
				}
				dxpro[para.vidsub2[0]] += dxpro2[para.vidsub2[0]];
				dxpro[para.vidsub2[1]] += -dxpro2[para.vidsub2[0]];
				//second derivative
				// i == j, inside the interation sublattice, ortherwise is zero
				if (para.order > 0) //0�ײ���, Ϊ0�������������
				{
					d2xpro[para.vidsub2[0]][para.vidsub2[0]] = 2 * xpro_all / yfracs[para.idsub2[0]]
						* para.order*pow(xdiff, para.order - 1);
					d2xpro[para.vidsub2[1]][para.vidsub2[1]] = -2 * xpro_all / yfracs[para.idsub2[1]]
						* para.order*pow(xdiff, para.order - 1);
					if (para.order > 1)
					{
						d2xpro2[para.vidsub2[0]][para.vidsub2[0]] = xpro_all*para.order
							*(para.order - 1)*pow(xdiff, para.order - 2);
					}
					d2xpro[para.vidsub2[0]][para.vidsub2[0]] += d2xpro2[para.vidsub2[0]][para.vidsub2[0]];
					d2xpro[para.vidsub2[1]][para.vidsub2[1]] += d2xpro2[para.vidsub2[0]][para.vidsub2[0]];
				}
				// i!=j, 
				// both i and j inside the interaction sublattice
				d2xpro[para.vidsub2[0]][para.vidsub2[1]] = xpro_all
					/ yfracs[para.idsub2[0]] / yfracs[para.idsub2[1]]
					* pow(xdiff, para.order);
				d2xpro2[para.vidsub2[0]][para.vidsub2[1]] = 0;
				d2xpro3[para.vidsub2[0]][para.vidsub2[1]] = 0;
				d2xpro4[para.vidsub2[0]][para.vidsub2[1]] = 0;
				if (para.order > 0)
				{
					d2xpro2[para.vidsub2[0]][para.vidsub2[1]] = xpro_all
						/ yfracs[para.idsub2[1]] * para.order*pow(xdiff, para.order - 1);
					d2xpro3[para.vidsub2[0]][para.vidsub2[1]] = -xpro_all
						/ yfracs[para.idsub2[0]] * para.order*pow(xdiff, para.order - 1);
					if (para.order > 1)
					{
						d2xpro4[para.vidsub2[0]][para.vidsub2[1]] = -xpro_all*para.order
							*(para.order - 1)*pow(xdiff, para.order - 2);
					}
				}
				d2xpro[para.vidsub2[0]][para.vidsub2[1]] += d2xpro2[para.vidsub2[0]][para.vidsub2[1]]
					+ d2xpro3[para.vidsub2[0]][para.vidsub2[1]]
					+ d2xpro4[para.vidsub2[0]][para.vidsub2[1]];
				d2xpro[para.vidsub2[1]][para.vidsub2[0]] = d2xpro[para.vidsub2[0]][para.vidsub2[1]];
				// only i inside the interaction sublattice
				//
				for (i = 0; i < para.vyn; i++)
				{
					for (j = i+1; j < para.vyn; j++)
					{
						d2xpro[para.vyidv[i]][para.vyidv[j]] = xpro_all / yfracs[para.vyidc[i]]
							/ yfracs[para.vyidc[j]] * pow(xdiff, para.order);
						if (para.vyidc[i] == para.vidsub2[0]&& para.vyidc[j] != para.vidsub2[1])
						{
							d2xpro2[para.vyidv[i]][para.vyidv[j]] = 0;

							if (para.order > 0)
							{
								d2xpro2[para.vyidv[i]][para.vyidv[j]] = xpro_all
									/ yfracs[para.vyidc[j]] * para.order*pow(xdiff, para.order - 1);
							}
							d2xpro[para.vyidv[i]][para.vyidv[j]] +=
								d2xpro2[para.vyidv[i]][para.vyidv[j]];
							d2xpro[para.vyidv[j]][para.vyidv[i]] = d2xpro[para.vyidv[i]][para.vyidv[j]];
						}
						else if(para.vyidc[i] == para.vidsub2[1] && para.vyidc[j] != para.vidsub2[0])
						{
							d2xpro2[para.vyidv[i]][para.vyidv[j]] = 0;

							if (para.order > 0)
							{
								d2xpro2[para.vyidv[i]][para.vyidv[j]] = -xpro_all
									/ yfracs[para.vyidc[j]] * para.order*pow(xdiff, para.order - 1);
							}
							d2xpro[para.vyidv[i]][para.vyidv[j]] +=
								d2xpro2[para.vyidv[i]][para.vyidv[j]];
							d2xpro[para.vyidv[j]][para.vyidv[i]] = d2xpro[para.vyidv[i]][para.vyidv[j]];
						}
						else if ((para.vyidc[i] != para.vidsub2[1] && para.vyidc[j] != para.vidsub2[0])
							&& (para.vyidc[i] != para.vidsub2[0] && para.vyidc[j] != para.vidsub2[1]))
						{
							d2xpro2[para.vyidv[i]][para.vyidv[j]] = 0;

							//both i and j are not inside the inter sub, part2 == 0;
							d2xpro[para.vyidv[i]][para.vyidv[j]] +=
								d2xpro2[para.vyidv[i]][para.vyidv[j]];
							d2xpro[para.vyidv[j]][para.vyidv[i]] = d2xpro[para.vyidv[i]][para.vyidv[j]];
						}
						
					}
				}
				break;
			case 3:  // ��Ԫ�໥���ò���
				xpro = 0;
				for (i = 0; i < para.vyn; i++)
				{
					dxpro[para.vyidv[i]] = 0;
					for (j = i + 1; j < para.vyn; j++)
					{
						d2xpro[para.vyidv[i]][para.vyidv[j]] = 0;
						d2xpro[para.vyidv[j]][para.vyidv[i]] = d2xpro[para.vyidv[i]][para.vyidv[j]];
					}
				}
				//�ǵ����ڲ���Ԫ�໥���ã���
				break;
			case 4:  // ��Ԫ�໥���ò���
				xpro = 0;
				for (i = 0; i < para.vyn; i++)
				{
					dxpro[i] = 0;
					for (j = i + 1; j < para.vyn; j++)
					{
						d2xpro[i][j] = 0;
						d2xpro[j][i] = d2xpro[i][j];
					}
				}
				//�ǵ����ڲ���Ԫ�໥���ã���
				break;
			default:
				break;
			}

			// �ۼӵ�Gibbs��������
			CalcPara(paravalue, Tnseg, para, T);
			if (para.type == "G" || para.type == "L")
			{
				Phases.GF += xpro*paravalue[0];
				Phases.dGFt += xpro*paravalue[1];
				Phases.d2GFt += xpro*paravalue[2];
				for (i = 0;i < para.vyn;i++)
				{
					Phases.dGFx[para.vyidv[i]] += dxpro[para.vyidv[i]] * paravalue[0];
					Phases.d2GFxt[para.vyidv[i]] += dxpro[para.vyidv[i]] * paravalue[1];
					for (j = i; j < para.vyn;j++)
					{
						Phases.d2GFx[para.vyidv[i]][para.vyidv[j]] += d2xpro[para.vyidv[i]][para.vyidv[j]] * paravalue[0];
						Phases.d2GFx[para.vyidv[j]][para.vyidv[i]] = Phases.d2GFx[para.vyidv[i]][para.vyidv[j]];
					}
				}
			}
			else if (para.type == "TC")
				TC += xpro*CalcPara(Tnseg, para, T);
			else if (para.type == "BMAGN")
				BMAGN += xpro*CalcPara(Tnseg, para, T);
		}
		//�洢���Բ����������������ʵļ��㣬����һ�ס����׵���
		Phases.TC = TC;
		Phases.BMAGN = BMAGN;
		//����
		tao = T / TC;
		//GF += R*T*log(BMAGN + 1)*CalcGtao(tao, Phases.magp, 1);
		
		// ��
		// ѭ�����е�y
		double natom;
		natom = 0;
		Phases.SF = 0;
		for (i = 0; i < Phases.yn; i++)
		{
			Phases.SF += Phases.ysp[i] * Phases.y[i] * log(Phases.y[i]);
			if (Phases.yide[i]>1)  //Calc the mole atom in this formular, skip vacancy
				natom += Phases.ysp[i] * Phases.y[i];
		}
		Phases.SF = R*T*Phases.SF;
		Phases.GF += Phases.SF;

		Phases.dSFt = Phases.SF / T;
		Phases.d2SFt = 0;
		Phases.dGFt += Phases.dSFt;
		Phases.d2GFt += Phases.d2SFt;
		for (i = 0; i < Phases.vyn; i++)
		{
			Phases.dSFx[i] = Phases.ysp[Phases.vyidc[i]] * (log(yfracs[Phases.vyidc[i]]) + 1);
			Phases.dSFx[i] = R*T*Phases.dSFx[i];
			Phases.dGFx[i] += Phases.dSFx[i];
			Phases.d2SFxt[i] = Phases.dSFx[i] / T;
			Phases.d2GFxt[i] += Phases.d2SFxt[i];
			//
			Phases.d2SFx[i][i] = Phases.ysp[Phases.vyidc[i]] /yfracs[Phases.vyidc[i]];
			Phases.d2SFx[i][i] = R*T*Phases.d2SFx[i][i];
			Phases.d2GFx[i][i] += Phases.d2SFx[i][i];
		}
		if(mode==11)
			Phases.GF = Phases.GF / natom;
	}

	//����Gibbs������
	double MinEnergy::CalcGF(Phase & Phases, double T)
	{
		//
		int i;
		double R = 8.31451;
		double xpro, xdiff;
		double GF = 0;
		double SF = 0;
		double TC = 0;
		double BMAGN = 0;
		double tao;
		//
		double yfracs[MDim];
		for (i = 0; i < Phases.yn; i++)
			yfracs[i] = Phases.y[i];

		// ѭ�������
		for (auto para : Phases.Parameters)
		{
			xpro = 1;
			xdiff = 1;
			// �ж��¶�����
			int Tnseg = 0;
			for (i = 0; i < para.T.size() - 1;i++)
			{
				if ((para.T[i] - 0.02)< T && T <(para.T[i+1] + 0.02))   // T - 0.02����ֹ298.14��298.15�������
					break;
				Tnseg++;
			}
			// ���������ǰ���ϵ��
			for (i = 0; i < para.yn; i++)
			{
				xpro = xpro*yfracs[para.yidc[i]];
			}
			// ���ݲ������ͣ���������
			switch (para.kind)
			{
			case 2:  // ��Ԫ�໥���ò���
					 //�ǵ����ڲ���Ԫ�໥���ã���(x1-x2)
				xdiff = yfracs[para.idsub2[0]] - yfracs[para.idsub2[1]];
				xpro = xpro*pow(xdiff, para.order);
				break;
			case 3:  // ��Ԫ�໥���ò���
				xpro = 0;
					 //�ǵ����ڲ���Ԫ�໥���ã���
				break;
			case 4:  // ��Ԫ�໥���ò���
				xpro = 0;
					 //�ǵ����ڲ���Ԫ�໥���ã���
				break;
			}

			// �ۼӵ�Gibbs��������
			if (para.type == "G" || para.type == "L")
				GF += xpro*CalcPara(Tnseg, para, T);
			else if (para.type == "TC")
				TC += xpro*CalcPara(Tnseg, para, T);
			else if (para.type == "BMAGN")
				BMAGN += xpro*CalcPara(Tnseg, para, T);
		}
		//�洢���Բ����������������ʵļ��㣬����һ�ס����׵���
		Phases.TC = TC;
		Phases.BMAGN = BMAGN;
		//����
		tao = T / TC;
		GF += R*T*log(BMAGN + 1)*CalcGtao(tao, Phases.magp, 1);

		// ��
		// ѭ�����е�y
		double natom;
		natom = 0;
		for (i = 0; i < Phases.yn; i++)
		{
			if (Phases.y[i]<eps)
				SF += Phases.ysp[i] *eps * log(eps);
			else
				SF += Phases.ysp[i] * Phases.y[i] * log(Phases.y[i]);

			if (Phases.yide[i]>1)  //��������λ
				natom += Phases.ysp[i] * Phases.y[i];
		}
		SF = R*T*SF;
		GF += SF;
		//GF = GF / natom;
		return GF;
	}
	//
	void MinEnergy::CalcGPGF(Phase & Phases, double T)
	{
		//
		int i, j, k;
		int GPn;
		double R = 8.31451;
		double *xpro, *xdiff;
		double paravalue;

		GPn = Phases.GPn;
		//����ռ�
		
		xpro = new double[GPn];
		xdiff = new double[GPn];
		//��ʼ��
		for (i = 0; i < GPn; i++)
		{
			Phases.GPGF[i] = 0;
			Phases.GPSF[i] = 0;
			Phases.GPTC[i] = 0;
			Phases.GPBMAGN[i] = 0;
		}
		if (Phases.magp != 0)
		{
			for (i = 0; i < GPn; i++)
			{
				Phases.GPTC[i] = 0;
				Phases.GPBMAGN[i] = 0;
			}
		}
		

		// ѭ�������
		for (auto para : Phases.Parameters)
		{
			for (i = 0; i < GPn; i++)
				xpro[i] = 1;
			// �ж��¶�����
			int Tnseg = 0;
			for (i = 0; i < para.T.size() - 1; i++)
			{
				if ((para.T[i] - 0.02)< T && T <(para.T[i + 1] + 0.02))   // T - 0.02����ֹ298.14��298.15�������
					break;
				Tnseg++;
			}
			// ���������ǰ���ϵ��
			for (i = 0; i < para.yn; i++)
				for (j = 0; j < Phases.GPn; j++)
					xpro[j] = xpro[j] * Phases.GP[j].y[para.yidc[i]];
			// ���ݲ������ͣ���������
			switch (para.kind)
			{
			case 2:  // ��Ԫ�໥���ò���
					 //�ǵ����ڲ���Ԫ�໥���ã���(x1-x2)
				for (j = 0; j < Phases.GPn; j++)
				{
					xdiff[j] = Phases.GP[j].y[para.idsub2[0]] - Phases.GP[j].y[para.idsub2[1]];
					xpro[j] = xpro[j] * pow(xdiff[j], para.order);
				}
				break;
			case 3:  // ��Ԫ�໥���ò���
				xpro = 0;

					 //�ǵ����ڲ���Ԫ�໥���ã���
				break;
			case 4:  // ��Ԫ�໥���ò���
				xpro = 0;

					 //�ǵ����ڲ���Ԫ�໥���ã���
				break;
			}

			// �ۼӵ�Gibbs��������
			paravalue = CalcPara(Tnseg, para, T);
			if (para.type == "G" || para.type == "L")
			{
				for (j = 0; j < GPn; j++)
					Phases.GPGF[j] += xpro[j] * paravalue;
			}
			else if (para.type == "TC")
			{
				for (j = 0; j < GPn; j++)
					Phases.GPTC[j] += xpro[j] * paravalue;
			}
			else if (para.type == "BMAGN")
			{
				for (j = 0; j < GPn; j++)
					Phases.GPBMAGN[j] += xpro[j] * paravalue;
			}
		}
		//�洢���Բ����������������ʵļ��㣬����һ�ס����׵���
		//����
		if (Phases.magp != 0)
		{
			for (j = 0; j < GPn; j++)
			{
				Phases.GPtao[j] = T / Phases.GPTC[j];
				Phases.GPGF[j] = R*T*log(Phases.GPBMAGN[j] + 1)*CalcGtao(Phases.GPtao[j], Phases.magp, 1);
			}
		}

		// ��
		//ѭ���ǵ���
		double natom;
		for (j = 0; j < GPn; j++)
		{
			natom = 0;
			for (k = 0; k < Phases.yn; k++)
			{
				if(Phases.GP[j].y[k]<eps)
					Phases.GPSF[j] += Phases.ysp[k] * eps * log(eps);
				else
					Phases.GPSF[j] += Phases.ysp[k] * Phases.GP[j].y[k] * log(Phases.GP[j].y[k]);

				if (Phases.yide[k]>1)  //��������λ
					natom += Phases.ysp[k] * Phases.GP[j].y[k];
			}
			Phases.GPSF[j] = R*T*Phases.GPSF[j];
			Phases.GPGF[j] += Phases.GPSF[j];
			//����1mol��ѧʽ�а�������Ħ��ԭ��������������λ
			Phases.GPGF[j] = Phases.GPGF[j]/natom;
		}

		//delete[] xpro;
		//delete[] xdiff;
	}

	// ����Gibbs������һ�׵���
	double MinEnergy::CalcGFdx(Phase & Phases, double T, int idy)
	{
		//
		int i;
		double R = 8.31451;
		double xpro, xpro2, xdiff;
		double dGF = 0;
		double dSF = 0;
		double dTC = 0;
		double dBMAGN = 0;
		double tao;

		//
		double yfracs[MDim];
		for (i = 0; i < Phases.yn; i++)
		{
			
			yfracs[i] = Phases.y[i];

		}

		//G��L
		for (auto para : Phases.Parameters)
		{
			//�ж�idy�Ƿ��ڲ����У����ڣ�����Ϊ0
			int checkcon = 0;
			int naddcon = 0;
			for (i = 0; i < para.yn; i++)
			{
				if (para.yidc[i] == idy)
					checkcon = 1;
			}
			if (checkcon == 0) //idy���ڲ����б��У��ò����Ե�һ�׵���Ϊ0��������
				continue;

			// �ж��¶�����
			int Tnseg = 0;
			for (i = 0; i < para.T.size() - 1; i++)
			{
				if ((para.T[i] - 0.02)< T && T <(para.T[i + 1] + 0.02))   // T - 0.02����ֹ298.14��298.15�������
					break;
				Tnseg++;
			}
			xpro = 1;
			xpro2 = 1;
			xdiff = 1;
			// ���������ǰ���ϵ��
			for (i = 0; i < para.yn; i++)
			{
				xpro = xpro*yfracs[para.yidc[i]];
			}
			xpro = xpro / yfracs[idy]; // %����֮��Ϊ1,��yfracs[idy]/yfracs[idy] =1��
									   // ���ݲ������ͣ���������
			switch (para.kind)
			{

			case 2:  // ��Ԫ�໥���ò���
					 //��һ����
					 //�ǵ����ڲ���Ԫ�໥���ã���(x1-x2)
				xdiff = yfracs[para.idsub2[0]] - yfracs[para.idsub2[1]];
				xpro = xpro*pow(xdiff, para.order);
				//�ڶ�����, idy���໥���õ��ǵ����ڲ���Xi*Xj*Xk(Xi-Xj)^n��idy��Xi����Xj
				if (para.order > 0)  // n=0,��֮��Ϊ0�������ǡ�
				{
					if (idy == para.idsub2[0])  // (Xi-Xj)^n, idy ==Xi
					{
						for (i = 0; i < Phases.subln + 1; i++)
						{
							xpro2 = xpro2*yfracs[para.yidc[i]];
						}
						xpro2 = xpro2*para.order*pow(xdiff, para.order - 1);
					}
					else if ((idy == para.idsub2[1]))  // (Xi-Xj)^n, idy ==Xj
					{
						for (i = 0; i < Phases.subln + 1; i++)
						{
							xpro2 = xpro2*yfracs[para.yidc[i]];
						}
						xpro2 = -xpro2*para.order*pow(xdiff, para.order - 1);
					}
					xpro += xpro2;
				}
				break;
			case 3:  // ��Ԫ�໥���ò���
				xpro = 0;

					 //�ǵ����ڲ���Ԫ�໥���ã���
				break;
			case 4:
				xpro = 0;
				break;
			}

			// �ۼӵ�Gibbs��������

			if (para.type == "G" || para.type == "L")
				dGF += xpro*CalcPara(Tnseg, para, T);
			else if (para.type == "TC")
				dTC += xpro*CalcPara(Tnseg, para, T);
			else if (para.type == "BMAGN")
				dBMAGN += xpro*CalcPara(Tnseg, para, T);
		}
		//����
		if (Phases.magp != 0)
		{
			tao = T / Phases.TC;
			dGF += R*T*(CalcGtao(tao, Phases.magp, 0) / (Phases.BMAGN + 1)*dBMAGN -
				T / Phases.TC / Phases.TC*log(Phases.BMAGN + 1)*CalcGtao(tao, Phases.magp, 1)*dTC);
		}

		// ��
		
			dSF += Phases.ysp[idy] * (log(yfracs[idy]) + 1);
		dSF = R*T*dSF;
		dGF += dSF;
		//����Ħ��ԭ�ӵ�G
		double natom;
		natom = 0;
		for (i = 0; i < Phases.yn; i++)
		{
			if (Phases.yide[i]>1)  //��������λ
				natom += Phases.ysp[i] * Phases.y[i];
		}
		//dGF = dGF / natom;

		return dGF;
	}
	// ����Gibbs�����ܶ��׵���
	double MinEnergy::CalcGFdx2(Phase & Phases, double T, int idy1, int idy2)
	{
		//
		int i;
		double R = 8.31451;
		double xpro, xpro2, xpro3, xpro4, xdiff;
		int signx1, signx2;
		double dGF2 = 0;
		double dSF2 = 0;
		int checkcon1, checkcon2;

		//
		double yfracs[MDim];
		for (i = 0; i < Phases.yn; i++)
			yfracs[i] = Phases.y[i];

		//G��L
		for (auto para : Phases.Parameters)
		{
			//�ж�idy�Ƿ��ڲ����У����ڣ�����Ϊ0
			checkcon1 = -1;
			checkcon2 = -1;
			for (i = 0; i < para.yn; i++)
			{
				if (para.yidc[i] == idy1)
					checkcon1 = 1;
				if (para.yidc[i] == idy2)
					checkcon2 = 1;
			}
			if (checkcon1 == -1 || checkcon2 == -1) //idy1��idy2��ֻҪ��һ�������б��У��ò����ԵĶ��׵���Ϊ0��������
				continue;
			// �ж��¶�����
			int Tnseg = 0;
			for (i = 0; i < para.T.size() - 1; i++)
			{
				if ((para.T[i] - 0.02)< T && T <(para.T[i + 1] + 0.02))   // T - 0.02����ֹ298.14��298.15�������
					break;
				Tnseg++;
			}
			// �ɷ�ϵ��
			xpro = 1;
			xpro2 = 0;
			xpro3 = 0;
			xpro4 = 0;
			// �ж϶��׵�������
			if (idy1 == idy2)
			{
				switch (para.kind)
				{
				case 1:
					continue;  // end member�� ���׵���Ϊ0�������������
					break;
				case 2:
					if (para.order == 0)
						continue;  //0�ײ���, Ϊ0�������������
								   //�󵼺�ķ���
					signx1 = 0;
					if (idy1 == para.idsub2[0])
						signx1 = 1;
					else
						signx1 = -1;
					if (signx1 == 0)
						continue; //�����໥���õ��ǵ�����, Ϊ0�������������
								  //��һ����
					for (i = 0; i < para.yn; i++)
					{
						xpro = xpro*yfracs[para.yidc[i]];
					}
					xdiff = yfracs[para.idsub2[0]] - yfracs[para.idsub2[1]];  //�ǵ����ڲ���Ԫ�໥���ã���(x1-x2)
					if (para.order > 1)
						xpro2 = xpro* para.order*(para.order - 1)*pow(xdiff, para.order - 2);  // yi*yj*yk*n*(n-1)*(yi-yk)^(n-2)
					xpro = signx1 * 2 * xpro / yfracs[idy1] * para.order*pow(xdiff, para.order - 1); // 2*yj*yk*n*(yi-yk)^(n-1)
					xpro = xpro + xpro2;
					break;
				case 3:
					xpro = 0;
					break;
				case 4:
					xpro = 0;
					break;
				}
			}
			else    // idy1 != idy2
			{
				switch (para.kind)
				{
				case 1:   // end member  yk*G(i:j:k)
					for (i = 0; i < para.yn; i++)
					{
						xpro = xpro*yfracs[para.yidc[i]];
					}
					xpro = xpro / yfracs[idy1] / yfracs[idy2];; // %����֮��Ϊ1
					break;
				case 2:  // ��Ԫ�໥���ò���
						 //��һ����
					for (i = 0; i < para.yn; i++)
					{
						xpro = xpro*yfracs[para.yidc[i]];
					}
					xdiff = yfracs[para.idsub2[0]] - yfracs[para.idsub2[1]];
					//�ڶ�����
					//�󵼺�ķ���
					signx1 = 0;
					signx2 = 0;
					if (idy1 == para.idsub2[0])
						signx1 = 1;
					else if (idy1 == para.idsub2[1])
						signx1 = -1;
					if (idy2 == para.idsub2[0])
						signx2 = 1;
					else if (idy2 == para.idsub2[1])
						signx2 = -1;
					if (signx1 != 0 && signx2 != 0) // ��һ������������໥�����ǵ����ڲ�
					{
						if (para.order > 0)
						{
							xpro2 = signx1*xpro / yfracs[idy2] * para.order*pow(xdiff, para.order - 1); // yi*yk*n*(yi-yj)^(n-1)
							xpro3 = signx2*xpro / yfracs[idy1] * para.order*pow(xdiff, para.order - 1); // -yj*yk*n*(yi-yj)^(n-1)
							if (para.order > 1)
								xpro4 = -xpro*para.order*(para.order - 1)*pow(xdiff, para.order - 2); // -yi*yj*yk*n*(n-1)*(yi-yj)^(n-2)
							xpro = xpro / yfracs[idy1] / yfracs[idy2] * pow(xdiff, para.order);  // yk*n*(yi-yj)^n
						}
						else
							xpro = xpro / yfracs[idy1] / yfracs[idy2] * pow(xdiff, para.order);  // yk*n*(yi-yj)^n
						xpro = xpro + xpro2 + xpro3 + xpro4;
					}
					else if (signx1 != 0 && signx2 == 0) // �ڶ��������ֻ��һ�����໥���õ��ǵ����ڲ�
					{
						if (para.order > 0)
							xpro2 = signx1*xpro / yfracs[idy2] * para.order*pow(xdiff, para.order - 1);
						xpro = xpro / yfracs[idy1] / yfracs[idy2] * pow(xdiff, para.order);
						xpro = xpro + xpro2;
					}
					else if (signx1 == 0 && signx2 != 0) // �ڶ��������ֻ��һ�����໥���õ��ǵ����ڲ�
					{
						if (para.order > 0)
							xpro2 = signx2*xpro*yfracs[idy1] * para.order*pow(xdiff, para.order - 1);
						xpro = xpro / yfracs[idy1] / yfracs[idy2] * pow(xdiff, para.order);
						xpro = xpro + xpro2;
					}
					else if (signx1 != 0 && signx2 != 0) // ������������������໥���õ��ǵ����ڲ�
					{
						xpro = xpro / yfracs[idy1] / yfracs[idy2] * pow(xdiff, para.order);
					}
					break;
				case 3:
					for (i = 0; i < para.yn; i++)
					{
						xpro = xpro*yfracs[para.yidc[i]];
					}
					xpro = xpro / yfracs[idy1] / yfracs[idy2];; // %����֮��Ϊ1
					break;
				case 4:
					for (i = 0; i < para.yn; i++)
					{
						xpro = xpro*yfracs[para.yidc[i]];
					}
					xpro = xpro / yfracs[idy1] / yfracs[idy2];; // %����֮��Ϊ1
					break;
				}

			}
			// �ۼӵ�Gibbs��������
			dGF2 += xpro*CalcPara(Tnseg, para, T);
		}
		// ��

		if (idy1 == idy2)
		{

			dSF2 += Phases.ysp[idy1] / yfracs[idy1];
		}
		dSF2 = R*T*dSF2;
		dGF2 += dSF2;
		//����Ħ��ԭ�ӵ�G
		double natom;
		natom = 0;
		for (i = 0; i < Phases.yn; i++)
		{
			if (Phases.yide[i]>1)  //��������λ
				natom += Phases.ysp[i] * Phases.y[i];
		}
		//dGF2 = dGF2 / natom;
		//if (dGF2 > 1e9)
		//dGF2 = 1e9;
		return dGF2;
	}
	//
	//
	double MinEnergy::CalcPara(int Tnseg, Parameter para, double T)
	{
		int k = 0;
		double paravalue = 0;
		for (auto coffT : para.express_digit[Tnseg].coffT)
		{
			paravalue += coffT*pow(T, para.express_digit[Tnseg].powerT[k]);
			k++;
		}
		// ����T*LN(T)
		for (auto coffTLNT : para.express_digit[Tnseg].coffTLNT)
		{
			paravalue += coffTLNT*T*log(T);
		}
		return paravalue;
	}
	//
	void MinEnergy::CalcPara(double pvalue[3], int Tnseg, Parameter para, double T)
	{
		int k = 0;
		double temp0, temp1;
		pvalue[0] = 0;
		pvalue[1] = 0;
		pvalue[2] = 0;
		for (auto coffT : para.express_digit[Tnseg].coffT)
		{
			temp0 = coffT*pow(T, para.express_digit[Tnseg].powerT[k]);
			pvalue[0] += temp0;
			if (para.express_digit[Tnseg].powerT[k] != 0)
			{
				temp1 = para.express_digit[Tnseg].powerT[k] * temp0 / T;
				pvalue[1] += temp1;
				if (para.express_digit[Tnseg].powerT[k] != 1)
				{
					pvalue[2] += (para.express_digit[Tnseg].powerT[k] - 1) *temp1 / T;
				}
			}
			k++;
		}
		// ����T*LN(T)
		for (auto coffTLNT : para.express_digit[Tnseg].coffTLNT)
		{
			pvalue[0] += coffTLNT*T*log(T);
			pvalue[1] += coffTLNT*(log(T) + 1);
			pvalue[2] += coffTLNT / T;
		}
	}
	double MinEnergy::CalcGtao(double tao, double p, int order)
	{
		double  Gtao;
		double A = 0.460444444444444 + 0.731893583724570*(1 / p - 1);
		if (order == 0)
		{
			if (tao < 1)
				Gtao = 1 - 1 / A*(0.564285714285714 / tao / p + 0.953722334004024*
					(1 / p - 1)*(pow(tao, 3) / 6 + pow(tao, 9) / 135 + pow(tao, 15) / 600));
			else
				Gtao = -1 / A*(pow(tao, -5) / 10 + pow(tao, -15) / 315 + pow(tao, -25) / 1500);
		}
		else if (order == 1)
		{
			if (tao < 1)
				Gtao = 1 - 1 / A*(0.564285714285714 / tao / p + 0.953722334004024*
					(1 / p - 1)*(pow(tao, 3) / 6 + pow(tao, 9) / 135 + pow(tao, 15) / 600));
			else
				Gtao = -1 / A*(pow(tao, -5) / 10 + pow(tao, -15) / 315 + pow(tao, -25) / 1500);
		}
		else if (order == 2)
		{
			if (tao < 1)
				Gtao = -1 / A*(0.564285714285714 / tao / tao / p + 0.953722334004024*
					(1 / p - 1)*(pow(tao, 2) / 3 + pow(tao, 8) / 15 + pow(tao, 14) / 40));
			else
				Gtao = 1 / A*(pow(tao, -6) / 2 + pow(tao, -16) / 21 + pow(tao, -26) / 60);
		}
		return Gtao;
	}

=======
#include "..\include\MinEnergy.h"

namespace VCLab
{
	using namespace std;
	/*---------------------------------------------------------------------*/
	/*                       ����Gibbs������                               */
	/*---------------------------------------------------------------------*/
	/*
	void MinEnergy::CalcGE(Phase & Phases, double T)
	double MinEnergy::CalcGF(Phase & Phases, double T)
	double MinEnergy::CalcGFdx(Phase & Phases, double T, int idy)
	double MinEnergy::CalcGFdx2(Phase & Phases, double T, int idy1, int idy2)
	double MinEnergy::CalcPara(int Tnseg, Parameter para, double T)
	double MinEnergy::CalcGtao(double tao, double p, int order)
	*/

	// Calc the Gibbs energy, the first , second derivative of site fraction and temperature
	void MinEnergy::CalcGE(Phase & Phases, double T)
	{
		//
		int i, j;
		double R = 8.31451;
		double xpro_all, xpro, xdiff;
		double dxpro[10], dxpro2[10];
		double d2xpro[10][10], d2xpro2[10][10], d2xpro3[10][10], d2xpro4[10][10];

		double TC = 0;
		double BMAGN = 0;
		double tao;
		//
		double yfracs[MDim];
		double paravalue[3];

		for (i = 0; i < Phases.yn; i++)
		{
			if(Phases.y[i]<eps)
				yfracs[i] = eps;
			else
				yfracs[i] = Phases.y[i];
		}
		//
		Phases.GF = 0; Phases.SF = 0;
		Phases.dGFt = 0; Phases.d2GFt = 0;
		for (i = 0;i < Phases.vyn;i++)
		{
			Phases.dGFx[i] = 0;
			Phases.d2GFxt[i] = 0;
			for (j = 0;j < Phases.vyn;j++)
			{
				Phases.d2GFx[i][j] = 0;
			}
		}

		// ѭ�������
		for (auto para : Phases.Parameters)
		{
			// �ж��¶�����
			int Tnseg = 0;
			for (i = 0; i < para.T.size() - 1;i++)
			{
				if ((para.T[i] - 0.02)< T && T <(para.T[i + 1] + 0.02))   // T - 0.02����ֹ298.14��298.15�������
					break;
				Tnseg++;
			}
			//
			for (i = 0;i < Phases.vyn;i++)
			{
				dxpro[i] = 0;
				d2xpro[i][i] = 0;
				d2xpro2[i][i] = 0;
				d2xpro3[i][i] = 0;
				d2xpro4[i][i] = 0;

				for (j = i + 1;j < Phases.vyn;j++)
				{
					d2xpro[i][j] = 0; d2xpro[j][i] = 0;
					d2xpro2[i][j] = 0; d2xpro2[j][i] = 0;
					d2xpro3[i][j] = 0; d2xpro3[j][i] = 0;
					d2xpro4[i][j] = 0; d2xpro4[j][i] = 0;
				}
			}
			// ���������ǰ���ϵ��
			xpro_all = 1;
			for (i = 0; i < para.yn; i++)
			{
				xpro_all = xpro_all*yfracs[para.yidc[i]];
			}
			// ���ݲ������ͣ���������
			switch (para.kind)
			{
			case 1:
				//g
				xpro = xpro_all;
				// first derivative
				for (i = 0;i < para.vyn;i++)
				{
					dxpro[para.vyidv[i]] = xpro_all / yfracs[para.vyidc[i]];
					// second derivate
					// for i==j, in case 1, this part is zero
					for (j = i + 1;j < para.vyn;j++)
					{
						d2xpro[para.vyidv[i]][para.vyidv[j]] = dxpro[para.vyidv[i]] / yfracs[para.vyidc[j]];
						d2xpro[para.vyidv[j]][para.vyidv[i]] = d2xpro[para.vyidv[i]][para.vyidv[j]];
					}
				}
				break;
			case 2:  // ��Ԫ�໥���ò���
					 //�ǵ����ڲ���Ԫ�໥���ã���(x1-x2)
				xpro = xpro_all;
				xdiff = yfracs[para.idsub2[0]] - yfracs[para.idsub2[1]];
				xpro = xpro*pow(xdiff, para.order);
				//
				//first derivative
				//�ǵ����ڲ���Ԫ�໥���ã���(x1-x2)
				for (i = 0;i < para.vyn;i++)
				{
					dxpro[para.vyidv[i]] = xpro_all/yfracs[para.vyidc[i]]*pow(xdiff, para.order);
				}
				//�ڶ�����, idy���໥���õ��ǵ����ڲ���Xi*Xj*Xk(Xi-Xj)^n��idy��Xi����Xj
				dxpro2[para.vidsub2[0]] = 0;
				if (para.order > 0)  // n=0,��֮��Ϊ0�������ǡ�
				{
					dxpro2[para.vidsub2[0]] = xpro_all*para.order*pow(xdiff, para.order - 1);
				}
				dxpro[para.vidsub2[0]] += dxpro2[para.vidsub2[0]];
				dxpro[para.vidsub2[1]] += -dxpro2[para.vidsub2[0]];
				//second derivative
				// i == j, inside the interation sublattice, ortherwise is zero
				if (para.order > 0) //0�ײ���, Ϊ0�������������
				{
					d2xpro[para.vidsub2[0]][para.vidsub2[0]] = 2 * xpro_all / yfracs[para.idsub2[0]]
						* para.order*pow(xdiff, para.order - 1);
					d2xpro[para.vidsub2[1]][para.vidsub2[1]] = -2 * xpro_all / yfracs[para.idsub2[1]]
						* para.order*pow(xdiff, para.order - 1);
					if (para.order > 1)
					{
						d2xpro2[para.vidsub2[0]][para.vidsub2[0]] = xpro_all*para.order
							*(para.order - 1)*pow(xdiff, para.order - 2);
					}
					d2xpro[para.vidsub2[0]][para.vidsub2[0]] += d2xpro2[para.vidsub2[0]][para.vidsub2[0]];
					d2xpro[para.vidsub2[1]][para.vidsub2[1]] += d2xpro2[para.vidsub2[0]][para.vidsub2[0]];
				}
				// i!=j, 
				// both i and j inside the interaction sublattice
				d2xpro[para.vidsub2[0]][para.vidsub2[1]] = xpro_all
					/ yfracs[para.idsub2[0]] / yfracs[para.idsub2[1]]
					* pow(xdiff, para.order);
				d2xpro2[para.vidsub2[0]][para.vidsub2[1]] = 0;
				d2xpro3[para.vidsub2[0]][para.vidsub2[1]] = 0;
				d2xpro4[para.vidsub2[0]][para.vidsub2[1]] = 0;
				if (para.order > 0)
				{
					d2xpro2[para.vidsub2[0]][para.vidsub2[1]] = xpro_all
						/ yfracs[para.idsub2[1]] * para.order*pow(xdiff, para.order - 1);
					d2xpro3[para.vidsub2[0]][para.vidsub2[1]] = -xpro_all
						/ yfracs[para.idsub2[0]] * para.order*pow(xdiff, para.order - 1);
					if (para.order > 1)
					{
						d2xpro4[para.vidsub2[0]][para.vidsub2[1]] = -xpro_all*para.order
							*(para.order - 1)*pow(xdiff, para.order - 2);
					}
				}
				d2xpro[para.vidsub2[0]][para.vidsub2[1]] += d2xpro2[para.vidsub2[0]][para.vidsub2[1]]
					+ d2xpro3[para.vidsub2[0]][para.vidsub2[1]]
					+ d2xpro4[para.vidsub2[0]][para.vidsub2[1]];
				d2xpro[para.vidsub2[1]][para.vidsub2[0]] = d2xpro[para.vidsub2[0]][para.vidsub2[1]];
				// only i inside the interaction sublattice
				//
				for (i = 0; i < para.vyn; i++)
				{
					for (j = i+1; j < para.vyn; j++)
					{
						d2xpro[para.vyidv[i]][para.vyidv[j]] = xpro_all / yfracs[para.vyidc[i]]
							/ yfracs[para.vyidc[j]] * pow(xdiff, para.order);
						if (para.vyidc[i] == para.vidsub2[0]&& para.vyidc[j] != para.vidsub2[1])
						{
							d2xpro2[para.vyidv[i]][para.vyidv[j]] = 0;

							if (para.order > 0)
							{
								d2xpro2[para.vyidv[i]][para.vyidv[j]] = xpro_all
									/ yfracs[para.vyidc[j]] * para.order*pow(xdiff, para.order - 1);
							}
							d2xpro[para.vyidv[i]][para.vyidv[j]] +=
								d2xpro2[para.vyidv[i]][para.vyidv[j]];
							d2xpro[para.vyidv[j]][para.vyidv[i]] = d2xpro[para.vyidv[i]][para.vyidv[j]];
						}
						else if(para.vyidc[i] == para.vidsub2[1] && para.vyidc[j] != para.vidsub2[0])
						{
							d2xpro2[para.vyidv[i]][para.vyidv[j]] = 0;

							if (para.order > 0)
							{
								d2xpro2[para.vyidv[i]][para.vyidv[j]] = -xpro_all
									/ yfracs[para.vyidc[j]] * para.order*pow(xdiff, para.order - 1);
							}
							d2xpro[para.vyidv[i]][para.vyidv[j]] +=
								d2xpro2[para.vyidv[i]][para.vyidv[j]];
							d2xpro[para.vyidv[j]][para.vyidv[i]] = d2xpro[para.vyidv[i]][para.vyidv[j]];
						}
						else if ((para.vyidc[i] != para.vidsub2[1] && para.vyidc[j] != para.vidsub2[0])
							&& (para.vyidc[i] != para.vidsub2[0] && para.vyidc[j] != para.vidsub2[1]))
						{
							d2xpro2[para.vyidv[i]][para.vyidv[j]] = 0;

							//both i and j are not inside the inter sub, part2 == 0;
							d2xpro[para.vyidv[i]][para.vyidv[j]] +=
								d2xpro2[para.vyidv[i]][para.vyidv[j]];
							d2xpro[para.vyidv[j]][para.vyidv[i]] = d2xpro[para.vyidv[i]][para.vyidv[j]];
						}
						
					}
				}
				break;
			case 3:  // ��Ԫ�໥���ò���
				xpro = 0;
				for (i = 0; i < para.vyn; i++)
				{
					dxpro[para.vyidv[i]] = 0;
					for (j = i + 1; j < para.vyn; j++)
					{
						d2xpro[para.vyidv[i]][para.vyidv[j]] = 0;
						d2xpro[para.vyidv[j]][para.vyidv[i]] = d2xpro[para.vyidv[i]][para.vyidv[j]];
					}
				}
				//�ǵ����ڲ���Ԫ�໥���ã���
				break;
			case 4:  // ��Ԫ�໥���ò���
				xpro = 0;
				for (i = 0; i < para.vyn; i++)
				{
					dxpro[i] = 0;
					for (j = i + 1; j < para.vyn; j++)
					{
						d2xpro[i][j] = 0;
						d2xpro[j][i] = d2xpro[i][j];
					}
				}
				//�ǵ����ڲ���Ԫ�໥���ã���
				break;
			default:
				break;
			}

			// �ۼӵ�Gibbs��������
			CalcPara(paravalue, Tnseg, para, T);
			if (para.type == "G" || para.type == "L")
			{
				Phases.GF += xpro*paravalue[0];
				Phases.dGFt += xpro*paravalue[1];
				Phases.d2GFt += xpro*paravalue[2];
				for (i = 0;i < para.vyn;i++)
				{
					Phases.dGFx[para.vyidv[i]] += dxpro[para.vyidv[i]] * paravalue[0];
					Phases.d2GFxt[para.vyidv[i]] += dxpro[para.vyidv[i]] * paravalue[1];
					for (j = i; j < para.vyn;j++)
					{
						Phases.d2GFx[para.vyidv[i]][para.vyidv[j]] += d2xpro[para.vyidv[i]][para.vyidv[j]] * paravalue[0];
						Phases.d2GFx[para.vyidv[j]][para.vyidv[i]] = Phases.d2GFx[para.vyidv[i]][para.vyidv[j]];
					}
				}
			}
			else if (para.type == "TC")
				TC += xpro*CalcPara(Tnseg, para, T);
			else if (para.type == "BMAGN")
				BMAGN += xpro*CalcPara(Tnseg, para, T);
		}
		//�洢���Բ����������������ʵļ��㣬����һ�ס����׵���
		Phases.TC = TC;
		Phases.BMAGN = BMAGN;
		//����
		tao = T / TC;
		//GF += R*T*log(BMAGN + 1)*CalcGtao(tao, Phases.magp, 1);
		
		// ��
		// ѭ�����е�y
		double natom;
		natom = 0;
		Phases.SF = 0;
		for (i = 0; i < Phases.yn; i++)
		{
			Phases.SF += Phases.ysp[i] * Phases.y[i] * log(Phases.y[i]);
			if (Phases.yide[i]>1)  //Calc the mole atom in this formular, skip vacancy
				natom += Phases.ysp[i] * Phases.y[i];
		}
		Phases.SF = R*T*Phases.SF;
		Phases.GF += Phases.SF;

		Phases.dSFt = Phases.SF / T;
		Phases.d2SFt = 0;
		Phases.dGFt += Phases.dSFt;
		Phases.d2GFt += Phases.d2SFt;
		for (i = 0; i < Phases.vyn; i++)
		{
			Phases.dSFx[i] = Phases.ysp[Phases.vyidc[i]] * (log(yfracs[Phases.vyidc[i]]) + 1);
			Phases.dSFx[i] = R*T*Phases.dSFx[i];
			Phases.dGFx[i] += Phases.dSFx[i];
			Phases.d2SFxt[i] = Phases.dSFx[i] / T;
			Phases.d2GFxt[i] += Phases.d2SFxt[i];
			//
			Phases.d2SFx[i][i] = Phases.ysp[Phases.vyidc[i]] /yfracs[Phases.vyidc[i]];
			Phases.d2SFx[i][i] = R*T*Phases.d2SFx[i][i];
			Phases.d2GFx[i][i] += Phases.d2SFx[i][i];
		}
		//GF = GF / natom;
	}

	//����Gibbs������
	double MinEnergy::CalcGF(Phase & Phases, double T)
	{
		//
		int i;
		double R = 8.31451;
		double xpro, xdiff;
		double GF = 0;
		double SF = 0;
		double TC = 0;
		double BMAGN = 0;
		double tao;
		//
		double yfracs[MDim];
		for (i = 0; i < Phases.yn; i++)
			yfracs[i] = Phases.y[i];

		// ѭ�������
		for (auto para : Phases.Parameters)
		{
			xpro = 1;
			xdiff = 1;
			// �ж��¶�����
			int Tnseg = 0;
			for (i = 0; i < para.T.size() - 1;i++)
			{
				if ((para.T[i] - 0.02)< T && T <(para.T[i+1] + 0.02))   // T - 0.02����ֹ298.14��298.15�������
					break;
				Tnseg++;
			}
			// ���������ǰ���ϵ��
			for (i = 0; i < para.yn; i++)
			{
				xpro = xpro*yfracs[para.yidc[i]];
			}
			// ���ݲ������ͣ���������
			switch (para.kind)
			{
			case 2:  // ��Ԫ�໥���ò���
					 //�ǵ����ڲ���Ԫ�໥���ã���(x1-x2)
				xdiff = yfracs[para.idsub2[0]] - yfracs[para.idsub2[1]];
				xpro = xpro*pow(xdiff, para.order);
				break;
			case 3:  // ��Ԫ�໥���ò���
				xpro = 0;
					 //�ǵ����ڲ���Ԫ�໥���ã���
				break;
			case 4:  // ��Ԫ�໥���ò���
				xpro = 0;
					 //�ǵ����ڲ���Ԫ�໥���ã���
				break;
			}

			// �ۼӵ�Gibbs��������
			if (para.type == "G" || para.type == "L")
				GF += xpro*CalcPara(Tnseg, para, T);
			else if (para.type == "TC")
				TC += xpro*CalcPara(Tnseg, para, T);
			else if (para.type == "BMAGN")
				BMAGN += xpro*CalcPara(Tnseg, para, T);
		}
		//�洢���Բ����������������ʵļ��㣬����һ�ס����׵���
		Phases.TC = TC;
		Phases.BMAGN = BMAGN;
		//����
		tao = T / TC;
		GF += R*T*log(BMAGN + 1)*CalcGtao(tao, Phases.magp, 1);

		// ��
		// ѭ�����е�y
		double natom;
		natom = 0;
		for (i = 0; i < Phases.yn; i++)
		{
			if (Phases.y[i]<eps)
				SF += Phases.ysp[i] *eps * log(eps);
			else
				SF += Phases.ysp[i] * Phases.y[i] * log(Phases.y[i]);

			if (Phases.yide[i]>1)  //��������λ
				natom += Phases.ysp[i] * Phases.y[i];
		}
		SF = R*T*SF;
		GF += SF;
		//GF = GF / natom;
		return GF;
	}
	//
	void MinEnergy::CalcGPGF(Phase & Phases, double T)
	{
		//
		int i, j, k;
		int GPn;
		double R = 8.31451;
		double *xpro, *xdiff;
		double paravalue;
		GPn = Phases.GPn;
		//����ռ�
		Phases.GPGF = new double[GPn];
		Phases.GPSF = new double[GPn];
		Phases.GPTC = new double[GPn];
		Phases.GPBMAGN = new double[GPn];
		Phases.GPtao = new double[GPn];
		xpro = new double[GPn];
		xdiff = new double[GPn];
		//��ʼ��
		for (i = 0; i < GPn; i++)
		{
			Phases.GPGF[i] = 0;
			Phases.GPSF[i] = 0;
			Phases.GPTC[i] = 0;
			Phases.GPBMAGN[i] = 0;
		}
		if (Phases.magp != 0)
		{
			for (i = 0; i < GPn; i++)
			{
				Phases.GPTC[i] = 0;
				Phases.GPBMAGN[i] = 0;
			}
		}
		

		// ѭ�������
		for (auto para : Phases.Parameters)
		{
			for (i = 0; i < GPn; i++)
				xpro[i] = 1;
			// �ж��¶�����
			int Tnseg = 0;
			for (i = 0; i < para.T.size() - 1; i++)
			{
				if ((para.T[i] - 0.02)< T && T <(para.T[i + 1] + 0.02))   // T - 0.02����ֹ298.14��298.15�������
					break;
				Tnseg++;
			}
			// ���������ǰ���ϵ��
			for (i = 0; i < para.yn; i++)
				for (j = 0; j < Phases.GPn; j++)
					xpro[j] = xpro[j] * Phases.GPy[j][para.yidc[i]];
			// ���ݲ������ͣ���������
			switch (para.kind)
			{
			case 2:  // ��Ԫ�໥���ò���
					 //�ǵ����ڲ���Ԫ�໥���ã���(x1-x2)
				for (j = 0; j < Phases.GPn; j++)
				{
					xdiff[j] = Phases.GPy[j][para.idsub2[0]] - Phases.GPy[j][para.idsub2[1]];
					xpro[j] = xpro[j] * pow(xdiff[j], para.order);
				}
				break;
			case 3:  // ��Ԫ�໥���ò���
				xpro = 0;

					 //�ǵ����ڲ���Ԫ�໥���ã���
				break;
			case 4:  // ��Ԫ�໥���ò���
				xpro = 0;

					 //�ǵ����ڲ���Ԫ�໥���ã���
				break;
			}

			// �ۼӵ�Gibbs��������
			paravalue = CalcPara(Tnseg, para, T);
			if (para.type == "G" || para.type == "L")
			{
				for (j = 0; j < GPn; j++)
					Phases.GPGF[j] += xpro[j] * paravalue;
			}
			else if (para.type == "TC")
			{
				for (j = 0; j < GPn; j++)
					Phases.GPTC[j] += xpro[j] * paravalue;
			}
			else if (para.type == "BMAGN")
			{
				for (j = 0; j < GPn; j++)
					Phases.GPBMAGN[j] += xpro[j] * paravalue;
			}
		}
		//�洢���Բ����������������ʵļ��㣬����һ�ס����׵���
		//����
		if (Phases.magp != 0)
		{
			for (j = 0; j < GPn; j++)
			{
				Phases.GPtao[j] = T / Phases.GPTC[j];
				Phases.GPGF[j] = R*T*log(Phases.GPBMAGN[j] + 1)*CalcGtao(Phases.GPtao[j], Phases.magp, 1);
			}
		}

		// ��
		//ѭ���ǵ���
		double natom;
		for (j = 0; j < GPn; j++)
		{
			natom = 0;
			for (k = 0; k < Phases.yn; k++)
			{
				if(Phases.GPy[j][k]<eps)
					Phases.GPSF[j] += Phases.ysp[k] * eps * log(eps);
				else
					Phases.GPSF[j] += Phases.ysp[k] * Phases.GPy[j][k] * log(Phases.GPy[j][k]);

				if (Phases.yide[k]>1)  //��������λ
					natom += Phases.ysp[k] * Phases.GPy[j][k];
			}
			Phases.GPSF[j] = R*T*Phases.GPSF[j];
			Phases.GPGF[j] += Phases.GPSF[j];
			//����1mol��ѧʽ�а�������Ħ��ԭ��������������λ
			Phases.GPGF[j] = Phases.GPGF[j]/natom;
		}
		delete[] Phases.GPSF;
		delete[] Phases.GPTC;
		delete[] Phases.GPBMAGN;
		delete[] Phases.GPtao;
		delete[] xpro;
		delete[] xdiff;
	}

	// ����Gibbs������һ�׵���
	double MinEnergy::CalcGFdx(Phase & Phases, double T, int idy)
	{
		//
		int i;
		double R = 8.31451;
		double xpro, xpro2, xdiff;
		double dGF = 0;
		double dSF = 0;
		double dTC = 0;
		double dBMAGN = 0;
		double tao;

		//
		double yfracs[MDim];
		for (i = 0; i < Phases.yn; i++)
		{
			
			yfracs[i] = Phases.y[i];

		}

		//G��L
		for (auto para : Phases.Parameters)
		{
			//�ж�idy�Ƿ��ڲ����У����ڣ�����Ϊ0
			int checkcon = 0;
			int naddcon = 0;
			for (i = 0; i < para.yn; i++)
			{
				if (para.yidc[i] == idy)
					checkcon = 1;
			}
			if (checkcon == 0) //idy���ڲ����б��У��ò����Ե�һ�׵���Ϊ0��������
				continue;

			// �ж��¶�����
			int Tnseg = 0;
			for (i = 0; i < para.T.size() - 1; i++)
			{
				if ((para.T[i] - 0.02)< T && T <(para.T[i + 1] + 0.02))   // T - 0.02����ֹ298.14��298.15�������
					break;
				Tnseg++;
			}
			xpro = 1;
			xpro2 = 1;
			xdiff = 1;
			// ���������ǰ���ϵ��
			for (i = 0; i < para.yn; i++)
			{
				xpro = xpro*yfracs[para.yidc[i]];
			}
			xpro = xpro / yfracs[idy]; // %����֮��Ϊ1,��yfracs[idy]/yfracs[idy] =1��
									   // ���ݲ������ͣ���������
			switch (para.kind)
			{

			case 2:  // ��Ԫ�໥���ò���
					 //��һ����
					 //�ǵ����ڲ���Ԫ�໥���ã���(x1-x2)
				xdiff = yfracs[para.idsub2[0]] - yfracs[para.idsub2[1]];
				xpro = xpro*pow(xdiff, para.order);
				//�ڶ�����, idy���໥���õ��ǵ����ڲ���Xi*Xj*Xk(Xi-Xj)^n��idy��Xi����Xj
				if (para.order > 0)  // n=0,��֮��Ϊ0�������ǡ�
				{
					if (idy == para.idsub2[0])  // (Xi-Xj)^n, idy ==Xi
					{
						for (i = 0; i < Phases.subln + 1; i++)
						{
							xpro2 = xpro2*yfracs[para.yidc[i]];
						}
						xpro2 = xpro2*para.order*pow(xdiff, para.order - 1);
					}
					else if ((idy == para.idsub2[1]))  // (Xi-Xj)^n, idy ==Xj
					{
						for (i = 0; i < Phases.subln + 1; i++)
						{
							xpro2 = xpro2*yfracs[para.yidc[i]];
						}
						xpro2 = -xpro2*para.order*pow(xdiff, para.order - 1);
					}
					xpro += xpro2;
				}
				break;
			case 3:  // ��Ԫ�໥���ò���
				xpro = 0;

					 //�ǵ����ڲ���Ԫ�໥���ã���
				break;
			case 4:
				xpro = 0;
				break;
			}

			// �ۼӵ�Gibbs��������

			if (para.type == "G" || para.type == "L")
				dGF += xpro*CalcPara(Tnseg, para, T);
			else if (para.type == "TC")
				dTC += xpro*CalcPara(Tnseg, para, T);
			else if (para.type == "BMAGN")
				dBMAGN += xpro*CalcPara(Tnseg, para, T);
		}
		//����
		if (Phases.magp != 0)
		{
			tao = T / Phases.TC;
			dGF += R*T*(CalcGtao(tao, Phases.magp, 0) / (Phases.BMAGN + 1)*dBMAGN -
				T / Phases.TC / Phases.TC*log(Phases.BMAGN + 1)*CalcGtao(tao, Phases.magp, 1)*dTC);
		}

		// ��
		
			dSF += Phases.ysp[idy] * (log(yfracs[idy]) + 1);
		dSF = R*T*dSF;
		dGF += dSF;
		//����Ħ��ԭ�ӵ�G
		double natom;
		natom = 0;
		for (i = 0; i < Phases.yn; i++)
		{
			if (Phases.yide[i]>1)  //��������λ
				natom += Phases.ysp[i] * Phases.y[i];
		}
		//dGF = dGF / natom;

		return dGF;
	}
	// ����Gibbs�����ܶ��׵���
	double MinEnergy::CalcGFdx2(Phase & Phases, double T, int idy1, int idy2)
	{
		//
		int i;
		double R = 8.31451;
		double xpro, xpro2, xpro3, xpro4, xdiff;
		int signx1, signx2;
		double dGF2 = 0;
		double dSF2 = 0;
		int checkcon1, checkcon2;

		//
		double yfracs[MDim];
		for (i = 0; i < Phases.yn; i++)
			yfracs[i] = Phases.y[i];

		//G��L
		for (auto para : Phases.Parameters)
		{
			//�ж�idy�Ƿ��ڲ����У����ڣ�����Ϊ0
			checkcon1 = -1;
			checkcon2 = -1;
			for (i = 0; i < para.yn; i++)
			{
				if (para.yidc[i] == idy1)
					checkcon1 = 1;
				if (para.yidc[i] == idy2)
					checkcon2 = 1;
			}
			if (checkcon1 == -1 || checkcon2 == -1) //idy1��idy2��ֻҪ��һ�������б��У��ò����ԵĶ��׵���Ϊ0��������
				continue;
			// �ж��¶�����
			int Tnseg = 0;
			for (i = 0; i < para.T.size() - 1; i++)
			{
				if ((para.T[i] - 0.02)< T && T <(para.T[i + 1] + 0.02))   // T - 0.02����ֹ298.14��298.15�������
					break;
				Tnseg++;
			}
			// �ɷ�ϵ��
			xpro = 1;
			xpro2 = 0;
			xpro3 = 0;
			xpro4 = 0;
			// �ж϶��׵�������
			if (idy1 == idy2)
			{
				switch (para.kind)
				{
				case 1:
					continue;  // end member�� ���׵���Ϊ0�������������
					break;
				case 2:
					if (para.order == 0)
						continue;  //0�ײ���, Ϊ0�������������
								   //�󵼺�ķ���
					signx1 = 0;
					if (idy1 == para.idsub2[0])
						signx1 = 1;
					else
						signx1 = -1;
					if (signx1 == 0)
						continue; //�����໥���õ��ǵ�����, Ϊ0�������������
								  //��һ����
					for (i = 0; i < para.yn; i++)
					{
						xpro = xpro*yfracs[para.yidc[i]];
					}
					xdiff = yfracs[para.idsub2[0]] - yfracs[para.idsub2[1]];  //�ǵ����ڲ���Ԫ�໥���ã���(x1-x2)
					if (para.order > 1)
						xpro2 = xpro* para.order*(para.order - 1)*pow(xdiff, para.order - 2);  // yi*yj*yk*n*(n-1)*(yi-yk)^(n-2)
					xpro = signx1 * 2 * xpro / yfracs[idy1] * para.order*pow(xdiff, para.order - 1); // 2*yj*yk*n*(yi-yk)^(n-1)
					xpro = xpro + xpro2;
					break;
				case 3:
					xpro = 0;
					break;
				case 4:
					xpro = 0;
					break;
				}
			}
			else    // idy1 != idy2
			{
				switch (para.kind)
				{
				case 1:   // end member  yk*G(i:j:k)
					for (i = 0; i < para.yn; i++)
					{
						xpro = xpro*yfracs[para.yidc[i]];
					}
					xpro = xpro / yfracs[idy1] / yfracs[idy2];; // %����֮��Ϊ1
					break;
				case 2:  // ��Ԫ�໥���ò���
						 //��һ����
					for (i = 0; i < para.yn; i++)
					{
						xpro = xpro*yfracs[para.yidc[i]];
					}
					xdiff = yfracs[para.idsub2[0]] - yfracs[para.idsub2[1]];
					//�ڶ�����
					//�󵼺�ķ���
					signx1 = 0;
					signx2 = 0;
					if (idy1 == para.idsub2[0])
						signx1 = 1;
					else if (idy1 == para.idsub2[1])
						signx1 = -1;
					if (idy2 == para.idsub2[0])
						signx2 = 1;
					else if (idy2 == para.idsub2[1])
						signx2 = -1;
					if (signx1 != 0 && signx2 != 0) // ��һ������������໥�����ǵ����ڲ�
					{
						if (para.order > 0)
						{
							xpro2 = signx1*xpro / yfracs[idy2] * para.order*pow(xdiff, para.order - 1); // yi*yk*n*(yi-yj)^(n-1)
							xpro3 = signx2*xpro / yfracs[idy1] * para.order*pow(xdiff, para.order - 1); // -yj*yk*n*(yi-yj)^(n-1)
							if (para.order > 1)
								xpro4 = -xpro*para.order*(para.order - 1)*pow(xdiff, para.order - 2); // -yi*yj*yk*n*(n-1)*(yi-yj)^(n-2)
							xpro = xpro / yfracs[idy1] / yfracs[idy2] * pow(xdiff, para.order);  // yk*n*(yi-yj)^n
						}
						else
							xpro = xpro / yfracs[idy1] / yfracs[idy2] * pow(xdiff, para.order);  // yk*n*(yi-yj)^n
						xpro = xpro + xpro2 + xpro3 + xpro4;
					}
					else if (signx1 != 0 && signx2 == 0) // �ڶ��������ֻ��һ�����໥���õ��ǵ����ڲ�
					{
						if (para.order > 0)
							xpro2 = signx1*xpro / yfracs[idy2] * para.order*pow(xdiff, para.order - 1);
						xpro = xpro / yfracs[idy1] / yfracs[idy2] * pow(xdiff, para.order);
						xpro = xpro + xpro2;
					}
					else if (signx1 == 0 && signx2 != 0) // �ڶ��������ֻ��һ�����໥���õ��ǵ����ڲ�
					{
						if (para.order > 0)
							xpro2 = signx2*xpro*yfracs[idy1] * para.order*pow(xdiff, para.order - 1);
						xpro = xpro / yfracs[idy1] / yfracs[idy2] * pow(xdiff, para.order);
						xpro = xpro + xpro2;
					}
					else if (signx1 != 0 && signx2 != 0) // ������������������໥���õ��ǵ����ڲ�
					{
						xpro = xpro / yfracs[idy1] / yfracs[idy2] * pow(xdiff, para.order);
					}
					break;
				case 3:
					for (i = 0; i < para.yn; i++)
					{
						xpro = xpro*yfracs[para.yidc[i]];
					}
					xpro = xpro / yfracs[idy1] / yfracs[idy2];; // %����֮��Ϊ1
					break;
				case 4:
					for (i = 0; i < para.yn; i++)
					{
						xpro = xpro*yfracs[para.yidc[i]];
					}
					xpro = xpro / yfracs[idy1] / yfracs[idy2];; // %����֮��Ϊ1
					break;
				}

			}
			// �ۼӵ�Gibbs��������
			dGF2 += xpro*CalcPara(Tnseg, para, T);
		}
		// ��

		if (idy1 == idy2)
		{

			dSF2 += Phases.ysp[idy1] / yfracs[idy1];
		}
		dSF2 = R*T*dSF2;
		dGF2 += dSF2;
		//����Ħ��ԭ�ӵ�G
		double natom;
		natom = 0;
		for (i = 0; i < Phases.yn; i++)
		{
			if (Phases.yide[i]>1)  //��������λ
				natom += Phases.ysp[i] * Phases.y[i];
		}
		//dGF2 = dGF2 / natom;
		//if (dGF2 > 1e9)
		//dGF2 = 1e9;
		return dGF2;
	}
	//
	//
	double MinEnergy::CalcPara(int Tnseg, Parameter para, double T)
	{
		int k = 0;
		double paravalue = 0;
		for (auto coffT : para.express_digit[Tnseg].coffT)
		{
			paravalue += coffT*pow(T, para.express_digit[Tnseg].powerT[k]);
			k++;
		}
		// ����T*LN(T)
		for (auto coffTLNT : para.express_digit[Tnseg].coffTLNT)
		{
			paravalue += coffTLNT*T*log(T);
		}
		return paravalue;
	}
	//
	void MinEnergy::CalcPara(double pvalue[3], int Tnseg, Parameter para, double T)
	{
		int k = 0;
		double temp0, temp1;
		pvalue[0] = 0;
		pvalue[1] = 0;
		pvalue[2] = 0;
		for (auto coffT : para.express_digit[Tnseg].coffT)
		{
			temp0 = coffT*pow(T, para.express_digit[Tnseg].powerT[k]);
			pvalue[0] += temp0;
			if (para.express_digit[Tnseg].powerT[k] != 0)
			{
				temp1 = para.express_digit[Tnseg].powerT[k] * temp0 / T;
				pvalue[1] += temp1;
				if (para.express_digit[Tnseg].powerT[k] != 1)
				{
					pvalue[2] += (para.express_digit[Tnseg].powerT[k] - 1) *temp1 / T;
				}
			}
			k++;
		}
		// ����T*LN(T)
		for (auto coffTLNT : para.express_digit[Tnseg].coffTLNT)
		{
			pvalue[0] += coffTLNT*T*log(T);
			pvalue[1] += coffTLNT*(log(T) + 1);
			pvalue[2] += coffTLNT / T;
		}
	}
	double MinEnergy::CalcGtao(double tao, double p, int order)
	{
		double  Gtao;
		double A = 0.460444444444444 + 0.731893583724570*(1 / p - 1);
		if (order == 0)
		{
			if (tao < 1)
				Gtao = 1 - 1 / A*(0.564285714285714 / tao / p + 0.953722334004024*
					(1 / p - 1)*(pow(tao, 3) / 6 + pow(tao, 9) / 135 + pow(tao, 15) / 600));
			else
				Gtao = -1 / A*(pow(tao, -5) / 10 + pow(tao, -15) / 315 + pow(tao, -25) / 1500);
		}
		else if (order == 1)
		{
			if (tao < 1)
				Gtao = 1 - 1 / A*(0.564285714285714 / tao / p + 0.953722334004024*
					(1 / p - 1)*(pow(tao, 3) / 6 + pow(tao, 9) / 135 + pow(tao, 15) / 600));
			else
				Gtao = -1 / A*(pow(tao, -5) / 10 + pow(tao, -15) / 315 + pow(tao, -25) / 1500);
		}
		else if (order == 2)
		{
			if (tao < 1)
				Gtao = -1 / A*(0.564285714285714 / tao / tao / p + 0.953722334004024*
					(1 / p - 1)*(pow(tao, 2) / 3 + pow(tao, 8) / 15 + pow(tao, 14) / 40));
			else
				Gtao = 1 / A*(pow(tao, -6) / 2 + pow(tao, -16) / 21 + pow(tao, -26) / 60);
		}
		return Gtao;
	}

>>>>>>> b34a88e731ed6049f6c364b37b469ae02b913009
}// end of namespace