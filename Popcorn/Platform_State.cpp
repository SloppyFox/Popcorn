#include "Platform_State.h"

// AsPlatform_State
//------------------------------------------------------------------------------------------------------------
AsPlatform_State::AsPlatform_State()
	:Regular(EPlatform_Substate_Regular::Missing), Rolling(EPlatform_Substate_Rolling::Unknown),
	Meltdown(EPlatform_Substate_Meltdown::Unknown), Glue(EPlatform_Transformation::Unknown),
	Expanding(EPlatform_Transformation::Unknown), Laser(EPlatform_Transformation::Unknown),
	Moving(EPlatform_Moving_State::Stop), Current_State(EPlatform_State::Regular),
	Next_State(EPlatform_State::Unknown)
{}
//------------------------------------------------------------------------------------------------------------
AsPlatform_State::operator EPlatform_State() const
{
	return Current_State;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_State::operator = (EPlatform_State new_state)
{
	Current_State = new_state;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_State::Set_Next_State(EPlatform_State next_state)
{
	if (Current_State == next_state /*|| Next_State == next_state*/)
		return;

	switch (Current_State)
	{
	case EPlatform_State::Regular:
		AsTools::Throw();  // для перехода в состояние Regular есть спец метод AsPlatform::Set_State(EPlatform_Substate_Regular new_regular_state)
		return;

	case EPlatform_State::Rolling:
		AsTools::Throw();  // в момент рестарта уровня платформа сама перейдет в нужное ей состояние
		return;

	case EPlatform_State::Meltdown:
		return;  // после Meltdown рестарт уровня. нельзя регистрировать новые состояния

	case EPlatform_State::Glue:
		Glue = EPlatform_Transformation::Finalize;
		break;

	case EPlatform_State::Expanding:
		Expanding = EPlatform_Transformation::Finalize;
		break;

	case EPlatform_State::Laser:
		Laser = EPlatform_Transformation::Finalize;
		break;

	default:
		AsTools::Throw();
	}

	Next_State = next_state;
}
//------------------------------------------------------------------------------------------------------------
EPlatform_State AsPlatform_State::Get_Next_State() const
{
	return Next_State;
}
//------------------------------------------------------------------------------------------------------------
EPlatform_State AsPlatform_State::Set_Next_Or_Regular_State(EPlatform_Substate_Regular new_regular_substate)
{
	Current_State = EPlatform_State::Regular;

	if (Next_State == EPlatform_State::Unknown)
		Regular = new_regular_substate;

	/*Next_State = EPlatform_State::Unknown;*/  //!!!

	return Next_State;
}
//------------------------------------------------------------------------------------------------------------
EPlatform_State AsPlatform_State::Set_State(EPlatform_Substate_Regular new_regular_substate)
{
	EPlatform_Transformation *transformation_state = 0;

	if (Current_State == EPlatform_State::Regular && Regular == new_regular_substate)
		return EPlatform_State::Unknown;

	if (new_regular_substate == EPlatform_Substate_Regular::Normal)
	{
		switch (Current_State)
		{
		case EPlatform_State::Glue:
			transformation_state = &Glue;
			break;

		case EPlatform_State::Expanding:
			transformation_state = &Expanding;
			break;

		case EPlatform_State::Laser:
			transformation_state = &Laser;
			break;
		}

		if (transformation_state != 0)
		{
			if (*transformation_state == EPlatform_Transformation::Unknown)
				return Set_Next_Or_Regular_State(new_regular_substate);
			else
				*transformation_state = EPlatform_Transformation::Finalize;

			return EPlatform_State::Unknown;
		}
	}

	Current_State = EPlatform_State::Regular;
	Regular = new_regular_substate;

	return EPlatform_State::Unknown;
}
//------------------------------------------------------------------------------------------------------------
