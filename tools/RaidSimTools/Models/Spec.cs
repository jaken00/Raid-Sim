using System.Text.Json.Serialization;

namespace RaidSimTools.Models;

public class SpecFile
{
    [JsonPropertyName("specializations")]
    public List<Spec> Specializations { get; set; } = [];
}

public class Spec
{
    [JsonPropertyName("class")]
    public string Class { get; set; } = "";

    [JsonPropertyName("role")]
    public string Role { get; set; } = "";

    [JsonPropertyName("name")]
    public string Name { get; set; } = "";

    [JsonPropertyName("resource")]
    public string Resource { get; set; } = "";

    [JsonPropertyName("attack_range")]
    public string AttackRange { get; set; } = "";

    [JsonPropertyName("dps_weight")]
    public double DpsWeight { get; set; }

    [JsonPropertyName("hps_weight")]
    public double HpsWeight { get; set; }

    [JsonPropertyName("defense_weight")]
    public double DefenseWeight { get; set; }

    [JsonPropertyName("utility_weight")]
    public double UtilityWeight { get; set; }

    [JsonPropertyName("primary_stat")]
    public string PrimaryStat { get; set; } = "";

    [JsonPropertyName("can_interrupt")]
    public bool CanInterrupt { get; set; }

    [JsonPropertyName("can_dispel")]
    public bool CanDispel { get; set; }

    [JsonPropertyName("provides_shield")]
    public bool ProvidesShield { get; set; }

    [JsonPropertyName("provides_external_cd")]
    public bool ProvidesExternalCd { get; set; }

    [JsonPropertyName("raid_buff")]
    public string RaidBuff { get; set; } = "";

    [JsonPropertyName("execute_bonus")]
    public double ExecuteBonus { get; set; }

    [JsonPropertyName("aoe_modifier")]
    public double AoeModifier { get; set; }

    [JsonPropertyName("spec_damage_type")]
    public string SpecDamageType { get; set; } = "";

    [JsonPropertyName("spec_stat_weight")]
    public SpecStatWeight StatWeight { get; set; } = new();

    [JsonPropertyName("fight_affinity_profile")]
    public FightAffinityProfile FightAffinity { get; set; } = new();
}

public class SpecStatWeight
{
    [JsonPropertyName("Haste")]
    public double Haste { get; set; }

    [JsonPropertyName("CriticalStrike")]
    public double CriticalStrike { get; set; }

    [JsonPropertyName("Expertise")]
    public double Expertise { get; set; }
}

public class FightAffinityProfile
{
    [JsonPropertyName("single_target_modifier")]
    public double SingleTargetModifier { get; set; }

    [JsonPropertyName("aoe_modifier")]
    public double AoeModifier { get; set; }

    [JsonPropertyName("cleave_modifier")]
    public double CleaveModifier { get; set; }

    [JsonPropertyName("movement_modifier")]
    public double MovementModifier { get; set; }

    [JsonPropertyName("execute_modifier")]
    public double ExecuteModifier { get; set; }

    [JsonPropertyName("melee_hostile_penalty")]
    public double MeleeHostilePenalty { get; set; }
}
