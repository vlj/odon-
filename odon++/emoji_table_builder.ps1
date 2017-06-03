# Taken From https://github.com/muan/emojilib/blob/master/emojis.json

$array = Get-Content emojis.txt -Encoding UTF8| ConvertFrom-Json | %{$_.psobject.properties} |
%{
    New-Object -Type PSCustomObject] -Property @{
    'Name' = $_.Name 
    'Char' = $_.Value.Char}
}

function BuildTree
{
    [cmdletbinding()] 
    param([PSCustomObject]$grp, [int]$i)

    Process {

        $subtable = $grp.Group | ?{ $_.Name.Length -gt $i } |
            Group-Object -Property {$_.Name.SubString($i, 1)} | select Name, Group 

        $processedTable = @()        
        $subtable | %{$processedTable += BuildTree -grp $_ -i ($i+1)}
        return  New-Object -Type PSCustomObject -Property @{
           'Letter' = $grp.Name
           'subtree' = $processedTable
           'leaves' = $grp.Group | ?{ $_.Name.Length -eq $i }
        }
    }
}

$table = $array | Group-Object -Property {$_.Name.Substring(0, 1)} | Select Name, Group |  %{BuildTree -grp $_ -i 1}

function CleanPrefix{
    [cmdletbinding()] 
    param([string]$inputString)

    Process {
        return $inputString | %{ $_ -replace "-", "_"} | %{ $_ -replace "\+", "P" }
    }

}


function TreeToTable {

    [cmdletbinding()] 
    param([System.Object[]]$tree, [string]$prefix)

    $result = ''
    foreach ($subtree in $tree.subtree)
    {
       $result += TreeToTable -tree $subtree -prefix ($prefix + (CleanPrefix -inputString $tree.Letter))
    }


    $result += "
    void check_" + $prefix + (CleanPrefix -inputString $tree.Letter) + "(utility::string_t& string_to_parse, const size_t& first_double_colon, size_t& current_position)
    {
        if (string_to_parse.size() <= current_position) return;
        const auto& current_char = string_to_parse[current_position];
        "
    if ($tree.leaves)
    {
        $result += "if (current_char == ':') { string_to_parse.replace(first_double_colon, " + ($prefix.Length + 3) + ', U("' + $tree.leaves.Char + '")' + "); current_position -= " + ($prefix.Length - 1) + "; return;}
        "
    }

    foreach ($subtree in $tree.subtree)
    {

        $result += "if (current_char == '" + $subtree.Letter + "') { check_" + $prefix + (CleanPrefix -inputString $tree.Letter) + (CleanPrefix -inputString $subtree.Letter) + "(string_to_parse, first_double_colon, ++current_position); return; }
        "
    }
        
    $result += "return;
    }"
    return $result
}

$globaltree = [PSCustomObject]@{'Letter' = ''
'subtree' = $table}
$code = TreeToTable -tree $globaltree -prefix ''
$code += "
    auto replace(const utility::string_t& str)
    {
        auto&& newstr = utility::string_t{str};
        size_t It = 0;
        for (; It < newstr.size(); ++It)
        {
            if (newstr[It] != ':') continue;
            auto start_point = It++;
            check_(newstr, start_point, It);
        }
        return newstr;
    }
"


$code | Set-Content emojimap.h -Encoding Unicode