#! perl

undef $/;
if ($ARGV[0] =~ /\.override$/) {
  $override = shift @ARGV;
  open IN, "$override" or die "Can't open $override\n";
  $override = <IN>;
  for $section (split /^\%\%\s*/mg, $override) {
    @lines = split /\n/, $section;
    $secname = shift @lines;
    if ($secname eq 'ignore') {
      $ignores = "|" . join("|", @lines) . "|";
    } elsif ($secname eq 'prefixes') {
      for $line (@lines) { ($a, $b) = split /\t/, $line;
                           $prefix{$a} = $b; push @prefixes, $a; }
    } elsif ($secname =~ /^function/) {
      ($junk, $name, $prefix, $package) = split /\t/, $secname;
      print "Found override function $name\n";
      push @{$funcs{$prefix}}, join("\n", @lines) . "\n\n";
    } elsif ($secname =~ /^init\s+(\w+)/) {
      $name = $1;
      print "Found init section for $name\n";
      $init{$name} = join("\n", @lines);
    } else {
      warn "Unkown section: $secname\n";
    }
  } # each section
} # if have override file

if ($ARGV[0] =~ /(\w+)\.defs/) {
  $modname = "$1";
} else {
  $modname = "Glib";
}

$info = <>;
for $defn ($info =~ /\(define-(\w+\s+([^()]+|[(]([^()]*|[(][^)]*[)])*[)])+)\)/sg) {
  next if ($defn =~ /^\s*$/s);
  $defn =~ s/GQuark/int/g;
  if ($defn =~ /^(\w+)\s+(\w+)/) {  $type = $1;   $name = $2;  }
  else { warn "Unparsable input: $defn"; next; }
  if ($defn =~ /\(in-module\s+"(\w+)"\)/) { $module = $1; }
  else { $module = "" }
  if ($defn =~ /\(parent\s+"(\w+)"\)/) { $parent = $1; }
  else { $parent = "" }
  if ($defn =~ /\(c-name\s+"(\w+)"\)/) { $cname = $1; }
  else { $cname = "" }
  if ($defn =~ /\(gtype-id\s+"(\w+)"\)/) { $gtypeid = $1; }
  else { $gtypeid = "" }
  if ($defn =~ /\(return-type\s+"([\w-]+\s*\*?)"\)/)
  {
    $ret = $1;
    $ret =~ s/-/ /;
  } 
  else { $ret = "" }
  if ($defn =~ /\(is-constructor-of\s+"(\w+)"\)/) { $construct = $1; }
  else { $construct = "" }
  if ($defn =~ /\(of-object\s+"(\w+)"\)/) { $obj = $1; }
  else { $obj = "" }
  if ($defn =~ /\(parameters\s+([^()]+|[(][^)]*[)])+\)/s)
  {
    $paramList = $&;
    @acc = ();
    @pacc = ();
    for $param ($paramList =~ /\((.*?)\)/g)
    {
      $param =~ s/-/ /;
      $param =~ /"([\w *]+)"\s+"(\w+)"/;
      push @acc, "$1 $2";
      push @pacc, "\$$2";
    }
    $params = join(", ", @acc);
    $pparams = join(", ", @pacc);
  } else { $params = ""; $pparams = ""; }

  $ret = "void" if ($ret eq 'none' or $ret eq '');

  $package = '';
  $myprefix = '';
  for $prefix (@prefixes) {
    if ($cname =~ /^$prefix(.+)$/) {
      $package = "::" . $prefix{$prefix};
      $myprefix = $prefix;
      last;
    }
  }

# FIXME: expand support for GEnum, GFlags, GBoxed
  if ($type eq 'object')
  { push @maps, "$gtypeid\t$cname\tGObject\t$modname"."::$name\n"; }
  if ($type eq 'function')
  {
    if ($ret eq 'GType')
    {
      print "Skipping function $name.\n";
      next;
    }
    if ($ignores =~ /\|$cname\|/)
    {
      print "Ignoring function $name.\n";
      next;
    }
    if ($construct) 
    {
      $ret =~ s/(\w+)/$1_noinc/;
      $pnames = $pparams;
      $pnames =~ s/\$//g;
      push @{$funcs{$myprefix}}, "## call as \$obj = new $modname$package"."::$name($pparams)\n" .
                   "$ret $cname (SV * class" .
                   ($params ? ", $params" : "") .
                   ")\n\tC_ARGS:\n\t\t$pnames" .
                   ($init{$cname} ? "\n    INIT:\n$init{$cname}" : "") . "\n\n";
    } else { 
      push @{$funcs{$myprefix}}, "## call as $modname$package"."::$name($pparams)\n" .
                   "$ret $cname ($params);" . 
                   ($init{$cname} ? "\n    INIT:\n$init{$cname}" : "") . "\n\n";
    }
  }
  if ($type eq 'method')
  {
    if ($ignores =~ /\|$cname\|/)
    {
      print "Ignoring method $cname.\n";
      next;
    }
    push @{$funcs{$myprefix}}, "## call as \$obj->$name($pparams)\n" .
                 "$ret $cname ($obj"." *self" .
                 ($params ? ", $params" : "") .
                 ");" . 
                 ($init{$cname} ? "\n    INIT:\n$init{$cname}" : "") . "\n\n";
  }
}

open OUT, ">$modname.maps" or die "Can't open >$modname.maps\n";
print OUT @maps;
close OUT;

open OUT, ">lib/$modname.xs" or die "Can't open >lib/$modname.xs\n";
print OUT <<"END";
#include <gperl.h>
#include <oscats.h>
#include "oscats-autogen.h"

SV * gperl_value_array_wrap(GType gtype, const char * package,
                            gpointer boxed, gboolean own)
{
  GValueArray *array = (GValueArray*)boxed;
  AV *av;
  SV *sv;
  guint i;
  
  av = newAV();
  for (i=0; i < array->n_values; i++)
  {
    GValue *value = array->values+i;
    sv = gperl_sv_from_value(value);
    av_push(av, sv);
    if (own)
    {
      if (G_VALUE_HOLDS_BOXED(value))
        g_boxed_free(G_VALUE_TYPE(value), g_value_get_boxed(value));
      else if (G_VALUE_HOLDS_OBJECT(value))
        g_object_unref(g_value_get_object(value));
    }
  }
  sv = newRV_noinc((SV*)av);

  return sv;
}

gpointer gperl_value_array_unwrap(GType gtype, const char *package, SV *sv)
{
  GValueArray *array;
  GValue value = {0, };
  AV *av;
  int i, num;
  
  g_return_val_if_fail(SvROK(sv) && SvTYPE(SvRV(sv)) == SVt_PVAV, NULL);
  av = (AV*) SvRV(sv);
  num = av_len(av)+1;
  
  array = g_value_array_new(num);
  for (i=0; i < num; i++)
  {
    SV **ee = av_fetch(av, i, FALSE);
    SV *e = (ee ? *ee : NULL);
    g_return_val_if_fail(e != NULL, NULL);

    if (SvROK(e))
    {
        g_value_init(&value, gperl_type_from_package(sv_reftype(SvRV(e), TRUE)));
        if (G_VALUE_HOLDS_OBJECT(&value))
          g_value_set_object(&value,
            gperl_get_object_check(e, G_VALUE_TYPE(&value)));
        else if (G_VALUE_HOLDS_BOXED(&value))
          g_value_set_boxed(&value,
            gperl_get_boxed_check(e, G_VALUE_TYPE(&value)));
        else
          g_warning("gperl_value_array_unwrap: What do I do here?");
    }
    else switch (SvTYPE(e))
    {
      case SVt_IV:	// int
        if (SvUOK(e))
        {
          g_value_init(&value, G_TYPE_INT);
          g_value_set_int(&value, SvIV(e));
        } else {
          g_value_init(&value, G_TYPE_UINT);
          g_value_set_uint(&value, SvUV(e));
        }
        break;
        
      case SVt_NV:	// double
        g_value_init(&value, G_TYPE_DOUBLE);
        g_value_set_double(&value, SvNV(e));
        break;
        
      case SVt_PV:	// string
        g_value_init(&value, G_TYPE_STRING);
        g_value_set_string(&value, SvPV_nolen(e));
        break;
        
      case SVt_PVMG:	// blessed/magical scalar
      case SVt_PVAV:	// array
      case SVt_PVHV:	// hash
      case SVt_PVCV:	// code
      case SVt_PVGV:	// glob
      default:
        g_warning("gperl_value_array_unwrap: Unsupported type for element %d", i);
    }
    
    g_value_array_append(array, &value);
    g_value_unset(&value);
  }

  return array;
}

void gperl_value_array_destroy (SV *sv)
{
  g_warning("gperl_value_array_destroy()");
}

GPerlBoxedWrapperClass valueArrayWrapper = {
  gperl_value_array_wrap,
  gperl_value_array_unwrap,
  gperl_value_array_destroy };

MODULE = $modname PACKAGE = $modname

BOOT:
#include "register-oscats.xsh"
  gperl_register_sink_func(OSCATS_TYPE_ALGORITHM, g_object_ref_sink);
  gperl_register_boxed(G_TYPE_VALUE_ARRAY, "ARRAY", &valueArrayWrapper);

END

for $prefix (keys %funcs) {
  if ($prefix) {
    print OUT "MODULE = $modname PACKAGE = $modname" . "::$prefix{$prefix} PREFIX = $prefix\n\n";
  } else {
    print OUT "MODULE = $modname PACKAGE = $modname\n\n";
  }
  print OUT join('', @{$funcs{$prefix}});
}
close OUT;
