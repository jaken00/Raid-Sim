using System.Text.Json.Serialization;

namespace RaidSimTools.Models;

public class SpellFile
{
    [JsonPropertyName("spells")]
    public List<Spell> Spells { get; set; } = [];
}

public class Spell
{
    [JsonPropertyName("id")]
    public int Id { get; set; }

    [JsonPropertyName("spell_name")]
    public string SpellName { get; set; } = "";

    [JsonPropertyName("mana_cost")]
    public int ManaCost { get; set; }

    [JsonPropertyName("damage_type")]
    public string DamageType { get; set; } = "";

    [JsonPropertyName("damage_value")]
    public int DamageValue { get; set; }

    [JsonPropertyName("heal_value")]
    public int HealValue { get; set; }

    [JsonPropertyName("provides_shield")]
    public bool ProvidesShield { get; set; }

    [JsonPropertyName("provides_buff")]
    public bool ProvidesBuff { get; set; }

    [JsonPropertyName("provides_debuff")]
    public bool ProvidesDebuff { get; set; }

    [JsonPropertyName("shield_id")]
    public int? ShieldId { get; set; }

    [JsonPropertyName("buff_id")]
    public int? BuffId { get; set; }

    [JsonPropertyName("debuff_id")]
    public int? DebuffId { get; set; }

    [JsonPropertyName("provides_dot")]
    public bool ProvidesDot { get; set; }

    [JsonPropertyName("dot_id")]
    public int? DotId { get; set; }
}
