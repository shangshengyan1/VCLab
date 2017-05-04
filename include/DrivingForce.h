<<<<<<< HEAD
/*
*   This file is a part of the VCLab software project.
*   For more details please contact xingwang@csu.edu.cn
*
*   Authors:    Xing Wang
*
*   Copyright (c) 2015-2015 Phase Diagram Center. Central South University. China
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*/
//

#ifndef DRIVINGFORCE_H_
#define DRIVINGFORCE_H_

#include "VCLInput.h"
#include "Database.h"
#include "Tool.h"

namespace VCLab
{
	using namespace std;
	//

	class DrivingForce
	{
	private:
		string classname;
		int linenumber;
	public:
		//
		Condition DFConditions;
		vector<Species> DFSpecies;
		vector<Phase> DFPhases;

		//
		int vn = 0;  //�ܱ�����
		double DF[MDim3];
		int npha = 2;  //����Ŀ
		double Chp[MDim]; //��ѧ��
		//int nsp = 4;  //��Ԫ��

					  // mode = -1 ȷ������������Driving Force ��Hillert's ƽ�ⷽ���鼰ţ�ٷ���Ҫ��ֵ������
		int mode = 1;

		//
		~DrivingForce();

		// ����ƽ��
		// ������������
		void SetupConditions(Condition CLCondition);
		// ȷ��Phase sets
		void SetupPhaseSets(Database CLDatabase);
		// ȷ��ϵͳ�����������
		void SetupVariables();
		// ����ϵͳ������ţ���ֵ��ÿ���༰��ѧ��
		void AssignVariables();
		// ��ʼ������ѡ�񣬵����������ѧ��
		void Initialization();
		// ȷ��Hillert's ƽ�ⷽ����,ţ�ٷ���ֵ������
		void SetupHillert();
		// ȷ������������Driving Force ��Hillert's ƽ�ⷽ���鼰ţ�ٷ���Ҫ��ֵ������
		void SetupDrivingForce();
		// ��������
		int Iteration();
		// �������� phase set
		void Solver(Database CLDatabase, Condition CLCondition);

		// print on screen
		void ShowEquilibrium();
		void ShowCondition();

	};

} // end of VCLab

=======
/*
*   This file is a part of the VCLab software project.
*   For more details please contact xingwang@csu.edu.cn
*
*   Authors:    Xing Wang
*
*   Copyright (c) 2015-2015 Phase Diagram Center. Central South University. China
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*/
//

#ifndef DRIVINGFORCE_H_
#define DRIVINGFORCE_H_

#include "VCLInput.h"
#include "Database.h"
#include "Tool.h"

namespace VCLab
{
	using namespace std;
	//

	class DrivingForce
	{
	private:
		string classname;
		int linenumber;
	public:
		//
		Condition DFConditions;
		vector<Species> DFSpecies;
		vector<Phase> DFPhases;

		//
		int vn = 0;  //�ܱ�����
		double DF[MDim3];
		int npha = 2;  //����Ŀ
		double Chp[MDim]; //��ѧ��
		//int nsp = 4;  //��Ԫ��

					  // mode = -1 ȷ������������Driving Force ��Hillert's ƽ�ⷽ���鼰ţ�ٷ���Ҫ��ֵ������
		int mode = 1;

		//
		~DrivingForce();

		// ����ƽ��
		// ������������
		void SetupConditions(Condition CLCondition);
		// ȷ��Phase sets
		void SetupPhaseSets(Database CLDatabase);
		// ȷ��ϵͳ�����������
		void SetupVariables();
		// ����ϵͳ������ţ���ֵ��ÿ���༰��ѧ��
		void AssignVariables();
		// ��ʼ������ѡ�񣬵����������ѧ��
		void Initialization();
		// ȷ��Hillert's ƽ�ⷽ����,ţ�ٷ���ֵ������
		void SetupHillert();
		// ȷ������������Driving Force ��Hillert's ƽ�ⷽ���鼰ţ�ٷ���Ҫ��ֵ������
		void SetupDrivingForce();
		// ��������
		int Iteration();
		// �������� phase set
		void Solver(Database CLDatabase, Condition CLCondition);

		// print on screen
		void ShowEquilibrium();
		void ShowCondition();

	};

} // end of VCLab

>>>>>>> b34a88e731ed6049f6c364b37b469ae02b913009
#endif