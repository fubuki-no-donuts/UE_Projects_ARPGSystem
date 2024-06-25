// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "InventoryBase.h"
#include "InventorySystem.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UInventorySystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Inventory", meta=(AllowPrivateAccess="true"))
	UItemBox* ItemBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Inventory", meta=(AllowPrivateAccess="true"))
	UItemBar* ItemBar;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Inventory", meta=(AllowPrivateAccess="true"))
	UEquipmentBox* EquipmentBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Inventory", meta=(AllowPrivateAccess="true"))
	UEquipBar* EquipBar;
};
