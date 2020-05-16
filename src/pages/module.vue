<template>
    <div>

        <!--
            显示模块的markdown
        -->
        <Markdown :markdown_name="markdown_name">
        </Markdown>

        <!--
            HomeLayout显示right， 用于选择section
        -->

    </div>
</template>

<script>

import Markdown from "components/Markdown";

export default {
  name: 'module',
  components: {
      Markdown : Markdown
  },
  data () {
    return {
        domain: this.$route.params.domain,
        module: this.$route.params.module
    }
  },
  methods: {
    
  },
  computed: {

      markdown_name () {
          return `${this.domain}/${this.module}/${this.$store.state.markdown.readme}`
      }

  },
  updated (){

    

  }
}
</script>

<style>
</style>
