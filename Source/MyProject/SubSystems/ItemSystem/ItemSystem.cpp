// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSystem.h"
#include "Engine/DataTable.h"

void UItemSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	AllItemsTable = LoadObject<UDataTable>(this, *AllItemsTablePath);
	AllEquipmentsTable = LoadObject<UDataTable>(this, *AllEquipmentsTablePath);
}

void UItemSystem::Deinitialize()
{
	Super::Deinitialize();

}

void UItemSystem::CheckItemByID(const int32 ItemID, bool& bIsExist) const
{
	bIsExist = false;
	if(AllItemsTable)
	{
		FName RowName = ItemTools::Int32ToFName(ItemID);
		FItemInfoTable* ItemTable = AllItemsTable->FindRow<FItemInfoTable>(RowName, RowName.ToString());
		bIsExist = (ItemTable != nullptr);
	}
}


