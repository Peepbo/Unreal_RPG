// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAdvancedAttackManager.h"
#include "AlgorithmStaticLibrary.h"

UEnemyAdvancedAttackManager::UEnemyAdvancedAttackManager()
{

}

void UEnemyAdvancedAttackManager::InitMontageData(TArray<FEnemyNormalAttack> NormalAttack, TArray<FEnemySpecialAttack> SpecialAttack, TArray<FEnemySpecialAttack> BackAttack, TArray<FEnemyMagicAttack> MagicAttack)
{
	NormalAttackList.Empty();
	SpecialAttackList.Empty();
	BackAttackList.Empty();
	MagicAttackList.Empty();

	if (bSequenceShuffle)
	{
		UAlgorithmStaticLibrary::ShuffleArray(NormalAttack);
		UAlgorithmStaticLibrary::ShuffleArray(SpecialAttack);
		UAlgorithmStaticLibrary::ShuffleArray(BackAttack);
		UAlgorithmStaticLibrary::ShuffleArray(MagicAttack);
	}

	for (const FEnemyNormalAttack& Attack : NormalAttack)
	{
		NormalAttackList.AddTail(Attack);
	}
	for (FEnemySpecialAttack& Attack : SpecialAttack)
	{
		if (Attack.bResetWaitCount)
		{
			Attack.WaitCount = Attack.MaximumWaitCount;
		}
		SpecialAttackList.AddTail(Attack);
	}
	for (FEnemySpecialAttack& Attack : BackAttack)
	{
		if (Attack.bResetWaitCount)
		{
			Attack.WaitCount = Attack.MaximumWaitCount;
		}
		BackAttackList.AddTail(Attack);
	}
	for (FEnemyMagicAttack& Attack : MagicAttack)
	{
		if (Attack.bResetWaitCount)
		{
			Attack.WaitCount = Attack.MaximumWaitCount;
		}
		MagicAttackList.AddTail(Attack);
	}
}

void UEnemyAdvancedAttackManager::SetAttackSequence(const TArray<EEnemyMontageType>& Sequence)
{
	if (Sequence.Num() > 0)
	{
		bool bSameType{ false };
		bool bIncludeMax{ false };
		bool bIncludeNormal{ false };

		// 같은 타입이 있는지 확인
		TSet<EEnemyMontageType> TypeSet;
		for (EEnemyMontageType Type : Sequence)
		{
			// 이미 존재한다면
			if (TypeSet.Contains(Type))
			{
				bSameType = true;
				break;
			}
			else
			{
				TypeSet.Add(Type);
			}
		}

		// Max가 포함됬는지 확인
		bIncludeMax = TypeSet.Contains(EEnemyMontageType::EEMT_MAX);
		// Normal이 포함됬는지 확인
		bIncludeNormal = TypeSet.Contains(EEnemyMontageType::EEMT_Normal);

		const bool bCopySequnce{ !bSameType && !bIncludeMax && !bIncludeNormal };
		if (bCopySequnce)
		{
			TypeSequence = Sequence;
		}
	}
	TypeSequence.Add(EEnemyMontageType::EEMT_Normal);
}

void UEnemyAdvancedAttackManager::ChooseAttack(float ToTargetDegree, float ToTargetDistance)
{
	if (TypeSequence.Num() > 0)
	{
		for (const EEnemyMontageType& Type : TypeSequence)
		{
			if (CheckAttackPossible(Type, ToTargetDegree, ToTargetDistance))
			{
				MontageType = Type;
				break;
			}
		}
	}
}

bool UEnemyAdvancedAttackManager::CheckAttackPossible(const EEnemyMontageType& Type, float ToTargetDegree, float ToTargetDistance)
{
	bool bValidMontage{ false };
	switch (Type)
	{
	case EEnemyMontageType::EEMT_Back:
		if (BackAttackList.GetHead())
		{
			EnemySpecialAttackLinkedListNode* ListNodePtr{ BackAttackList.GetHead() };
			FEnemySpecialAttack& AttackRef{ ListNodePtr->GetValue() };

			bValidMontage = AttackRef.AttackMontage != nullptr;
			if (bValidMontage)
			{
				bool bDegreeCondition{ abs(ToTargetDegree) >= 100.f };
				bool bDistanceCondition{ ToTargetDistance <= AttackRef.AttackAbleDistance };
				
				const bool bAttackCondition{ bDegreeCondition && bDistanceCondition };

				if (bAttackCondition)
				{
					if (AttackRef.WaitCount == 0)
					{
						AttackRef.WaitCount = AttackRef.MaximumWaitCount;
						BackAttackList.RemoveNode(ListNodePtr, false);
						BackAttackList.AddTail(ListNodePtr);

						AttackCommonDataPtr = &AttackRef;
						return true;
					}
					else
					{
						AttackRef.WaitCount--;
					}
				}
			}
		}
		break;
	case EEnemyMontageType::EEMT_Special:
		if (SpecialAttackList.GetHead())
		{
			EnemySpecialAttackLinkedListNode* ListNodePtr{ SpecialAttackList.GetHead() };
			FEnemySpecialAttack& AttackRef{ ListNodePtr->GetValue() };

			bValidMontage = AttackRef.AttackMontage != nullptr;
			if (bValidMontage)
			{
				if (AttackRef.WaitCount == 0)
				{
					AttackRef.WaitCount = AttackRef.MaximumWaitCount;
					SpecialAttackList.RemoveNode(ListNodePtr, false);
					SpecialAttackList.AddTail(ListNodePtr);

					AttackCommonDataPtr = &AttackRef;
					return true;
				}
				else
				{
					AttackRef.WaitCount--;
				}
			}
		}
		break;
	case EEnemyMontageType::EEMT_Magic:
		if (MagicAttackList.GetHead())
		{
			EnemyMagicAttackLinkedListNode* ListNodePtr{ MagicAttackList.GetHead() };
			FEnemyMagicAttack& AttackRef{ ListNodePtr->GetValue() };

			bValidMontage = AttackRef.AttackMontage != nullptr;
			if (bValidMontage)
			{
				if (AttackRef.WaitCount == 0)
				{
					AttackRef.WaitCount = AttackRef.MaximumWaitCount;
					MagicAttackList.RemoveNode(ListNodePtr, false);
					MagicAttackList.AddTail(ListNodePtr);

					AttackCommonDataPtr = &AttackRef;
					LastMagic = AttackRef.Magic;
					LastMagicDamage = AttackRef.MagicDamage;
					return true;
				}
				else
				{
					AttackRef.WaitCount--;
				}
			}
		}
		break;
	case EEnemyMontageType::EEMT_Normal:
		if (NormalAttackList.GetHead())
		{
			EnemyNormalAttackLinkedListNode* ListNodePtr{ NormalAttackList.GetHead() };
			FEnemyNormalAttack& AttackRef{ ListNodePtr->GetValue() };

			bValidMontage = AttackRef.AttackMontage != nullptr;
			if (bValidMontage)
			{
				NormalAttackList.RemoveNode(ListNodePtr, false);
				NormalAttackList.AddTail(ListNodePtr);

				AttackCommonDataPtr = &AttackRef;
				return true;
			}
		}
		break;
	}

	return false;
}